#!/usr/bin/perl
#
# Test suite runner script.
#
# This script allows running test suites in manner that takes care of
# tests that are stuck timing out and restarting test suite on
# next test when a test has failed.
# Logs resulting from test runs are captured and translated into
# on-screen test run display as well as (optionally) XML formatted log files.
#

##############################################################################
## Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
##
## This confidential and proprietary software may be used only as authorized
## by a licensing agreement from INSIDE Secure.
##
## The entire notice above must be reproduced on all authorized copies that
## may only be made to the extent permitted by a licensing agreement from
## INSIDE Secure.
##
## For more information or support, please go to our online support system at
## https://customersupport.insidesecure.com.
## In case you do not have an account for this system, please send an e-mail
## to ESSEmbeddedHW-Support@insidesecure.com.
##############################################################################

use warnings;
use strict;
use Getopt::Long;
use Pod::Usage;
use IO::Select;
use IPC::Open3;
use File::Spec;
use File::Temp qw/tempdir/;
use Symbol qw(gensym);
use Time::HiRes qw(time); # High resolution time

$main::VERSION = "1.0";
$|=1;

my $man = 0;
my $help = 0;
my $requested_suite=undef;
my $requested_tcase=undef;
my $requested_test=undef;
my $logfile_name=undef;
my $tempdir = tempdir("testrunner-tempXXXXXXXXXXXXXX",
                      TMPDIR => 1, CLEANUP => 1);
my $timeout=60;
my $verbose=0;
my $quiet=0;
my $test_state = 'NOT_RUNNING';
my $exitcode=undef;
my $child_pid=undef;
my $interpreter=undef;
my $interpreter_opts=undef;
my $argfile=undef;
my $xmllogfile =undef;
my $ldpreload=undef;
my $frequency=undef;
my $measurement=undef;
my $stackmeasurement=undef;
my $heapmeasurement=undef;
my $file_line=undef;
my $id=0;
my %expect_error = ();

my $endwait = 5;
my $xmlstart =0;
my $xmlsuite =undef;
my @xmlstack = ();

my $outlog = undef;
my $nointeract =undef;

my %short_opts;

my $linebuf = "";
my $result_unsupported_quick;

GetOptions("help|?" => \$help,
           "man" => \$man,
           "timeout=i" => \$timeout,
           "suite=s" => \$requested_suite,
           "tcase=s" => \$requested_tcase,
           "test=s" => \$requested_test,
           "log=s" => \$logfile_name,
           "xmllog=s" => \$xmllogfile,
           "outlog=s" => \$outlog,
           "verbose+" => \$verbose,
           "quiet+" => \$quiet,
           "timeout=i" => \$timeout,
           "interpreter=s" => \$interpreter,
           "interpreter-opts=s" => \$interpreter_opts,
           "frequency=i" => \$frequency,
           "ld-preload=s" => \$ldpreload,
           "use-argfile" => \$argfile,
           "end-wait=f" => \$endwait,
           "no-interact" => \$nointeract)
or pod2usage(2);

pod2usage(1) if $help;
pod2usage(-exitstatus => 0, -verbose => 2) if $man;

(print STDERR "Test executable name missing\n") &&
    pod2usage(-exitstatus => 1, -verbose => 1) if $#ARGV < 0;

my $exe = pop @ARGV;

my $current_suite=undef;
my $current_tcase=undef;
my $current_test=undef;
my $any_log=undef;
my %results;
my %results_comment;
my %results_tags;

my $outlogfile;
# Write entries to outlog/errlog
sub write_outlog
{
    my ($text) = @_;

    return if !defined $outlog;
    return if !defined $text or length $text == 0;
    if (!defined $outlogfile)
    {
        if ($outlog =~ /[<>|]/)
        {
            # Allow user to pass >>, |, ... to specify append or piping.
            open($outlogfile, $outlog) or die "Cannot open $outlog";
        }
        else
        {
            open($outlogfile, ">". $outlog) or die "Cannot open $outlog";
        }
        printf $outlogfile "%s\n%s%s\n%s\n",
        ":"x60, "Output-", $exe, ":"x60;
    }
    print $outlogfile $text;
}

# If exe does not contain any slash, add reference to local directory
$exe =~ s{^}{./} if $exe !~ m{/};

# For scaling test results
sub scale_measurement {
    my ($number, $units) = @_;
    my $cycles;
    my $nanos;

    # Autoprobe processor frequency if not provided
    if (!defined $frequency)
    {
        local $/;
        undef $/;
        eval
        {
            open CPUINFO, "</proc/cpuinfo";
            my $cpuinfo = <CPUINFO>;
            $frequency = $1 * 1000000 + $2 * 1000
                if $cpuinfo =~ /\ncpu MHz\s+:\s+(\d+)\.(\d{3})/s;
            close CPUINFO;
        }
    }
    if (!defined $frequency)
    {
        printf STDERR "No --frequency provided. Please provide --frequency.";
        exit(1);
    }

    printf STDERR "number=%s,units=%s,frequency=%s\n",
                  $number,$units,$frequency if $verbose >= 3;

    if ($units eq 'cycles')
    {
        $cycles = $number * 1.0;
        $nanos = ($number * 1000000000.0) / $frequency;
    }
    elsif ($units eq 'ns')
    {
        $nanos = $number * 1.0;
        $cycles = ($number / 1000000000.0) * $frequency;
    }
    else
    {
        printf STDERR "Unsupported units: $units";
        exit(1);
    }

    return [$cycles, $nanos];
}

#XML logfile generation, notice namespace is http://check.sourceforge.net/ns
#for compatibility with check
sub xmlopen
{
    return if !defined $xmllogfile;
    open(XMLLOGFILE, ">$xmllogfile") or
        die "Cannot open $xmllogfile for output";
    $xmlstart=1;
    xmlstarttag_internal('?xml', 'version' => '1.0', '?');
    xmltext("\n");
    xmlstarttag("testsuites", 'xmlns' => 'http://check.sourceforge.net/ns');
    xmltext("\n");
}

my %xmlencodetab =
(
    '<' => '&lt;',
    '>' => '&gt;',
    '&' => '&amp;',
    '"' => '&quot;'
);
sub xmlencode
{
    my $input = $_[0];
    $input =~ s/[<>&"]/$xmlencodetab{$&}/eg;
    return $input;
}

sub xmlstarttag_internal
{
    return if !$xmlstart;
    my @args = @_;

    shift @args; # Loose tag name from argument list
    print XMLLOGFILE "<".$_[0];

    while($#args > 0)
    {
        print XMLLOGFILE " ".(shift @args)."=".'"';
        print XMLLOGFILE xmlencode(shift @args);
        print XMLLOGFILE '"';
    }
    print XMLLOGFILE $args[0] if $#args > -1;
    print XMLLOGFILE ">";
}

sub xmlendtag_internal
{
    return if !$xmlstart;
    print XMLLOGFILE "</".$_[0].">\n";
}

sub xmlstarttag
{
    return if !$xmlstart;
    unshift @xmlstack, $_[0];
    xmlstarttag_internal(@_);
}

sub xmltag
{
    return if !$xmlstart;
    xmlstarttag_internal(@_, "/");
}

sub xmltext
{
    return if !$xmlstart;
    print XMLLOGFILE xmlencode($_[0]);
}

sub xmlendtag
{
    return if !$xmlstart;
    my $tag = shift @xmlstack;
    die "Tag mismatch" if $tag ne $_[0];
    xmlendtag_internal($tag);
}

sub xmloutput_suite
{
    my $suite = $_[0];
    return if !$xmlstart;
    return if defined $xmlsuite and $xmlsuite eq $suite;
    xmlendtag("suite") if defined $xmlsuite;

    xmlstarttag("suite");
    xmlstarttag("title");
    xmltext($suite);
    xmlendtag("title");
    $xmlsuite = $suite;
}

sub xmloutput_test
{
    my ($result, $fn, $id, $iteration, $description, $message,
        $extratags) = (@_);
    return if !$xmlstart;

    xmlstarttag("test", 'result' => $result);
    xmlstarttag("fn"); xmltext($fn); xmlendtag("fn");
    xmlstarttag("id"); xmltext($id); xmlendtag("id");
    xmlstarttag("iteration"); xmltext($iteration); xmlendtag("iteration");
    xmlstarttag("description"); xmltext($description); xmlendtag("description");
    xmlstarttag("message"); xmltext($message); xmlendtag("message");

    if (defined $extratags)
    {
        foreach my $extratags_key (keys %$extratags)
        {
            xmlstarttag($extratags_key);
            xmltext($extratags->{$extratags_key});
            xmlendtag($extratags_key);
        }
    }

    xmlendtag("test");
}

END {
    # Try to handle closing of open tags in xmllog.
    foreach my $tag (@xmlstack)
    {
        xmlendtag_internal($tag);
    }
}

# Helper for start_test and continue_test
sub do_start_test
{
    my @interpreter_opts=();
    my @cmdline_orig = @_;
    my $use_interpreter = pop @cmdline_orig;
    my @cmdline_begin;
    my @cmdline = @cmdline_orig;
    my $cmdline;

    @interpreter_opts = split(/ /, $interpreter_opts)
        if defined $interpreter_opts;

    @cmdline_begin=($cmdline_orig[0], $cmdline_orig[1],
                    @interpreter_opts) if $use_interpreter;
    @cmdline_begin=($cmdline_orig[0]) if !$use_interpreter;

    unshift @cmdline, ($interpreter, @interpreter_opts) if $use_interpreter;
    unshift @cmdline_orig, ($interpreter, @interpreter_opts)
        if $use_interpreter;

    @cmdline=map { s/'/'"'"'/; "'".$_."'"; } @cmdline;
    $cmdline=join(' ',@cmdline);
    print STDERR $cmdline."\n" if $verbose >= 1;
    print STDERR "Opening test handle\n" if $verbose >= 3;

    $id++;
    system "/bin/mv $tempdir/logs $tempdir/logs.$id 2>/dev/null";
    system "/bin/rm $tempdir/logs.$id 2>/dev/null";
    system "/usr/bin/mkfifo $tempdir/logs 2>/dev/null";
    $child_pid = fork();
    if ($child_pid == 0)
    {
        close(STDERR);
        open(STDERR, ">>$tempdir/logs");
        if (defined $argfile)
        {
            my @cmdline_args = @_;
            my $argfilename = "$tempdir/args.$$";
            open(ARGFILE, ">$argfilename");
            print ARGFILE join("\n", @cmdline_args)."\n";
            close(ARGFILE);
            close(STDIN);
            open(STDIN, "<$argfilename");
        }
        else
        {
            close(STDIN);
            open(STDIN, "</dev/null");
        }
        close(STDOUT);
        open(STDOUT, ">&STDERR");
        $| = 1;

        # Set LD_PRELOAD if requested.
        $ENV{LD_PRELOAD}=$ldpreload if defined $ldpreload;

        if (defined $argfile)
        {
            # Using argument file, use exec with only initial args
            exec { $cmdline_orig[0] } @cmdline_begin;
        }
        else
        {
            # No argument file, use exec with all arguments
            exec { $cmdline_orig[0] } @cmdline_orig;
        }
        die "Exec failed";
    }
    elsif (!defined $child_pid)
    {
        die "Fork failed.";
    }
    open(MONITORING_FILEHANDLE, "$tempdir/logs") or
        die "Unable to open log file.";
    print STDERR "Opened test handle [pid=$child_pid]\n" if $verbose >= 3;
}

# This begins running test on environment to test
sub start_test
{
    if (defined $nointeract)
    {
        open(MONITORING_FILEHANDLE, "$exe") or die "Unable to open log file.";
        return;
    }

    my @cmdline;
    push @cmdline, "$exe";
    push @cmdline, "--suite=$requested_suite" if defined $requested_suite;
    push @cmdline, "--tcase=$requested_tcase" if defined $requested_tcase;
    if (defined $requested_test)
    {
        if ($requested_test =~ /^(.*):(\d+)$/)
        {
            push @cmdline, "--test=$1";
            push @cmdline, "--iter=$2";
        }
        else
        {
            push @cmdline, "--test=$requested_test";
        }
    }
    do_start_test(@cmdline, defined $interpreter);
}

sub state_test
{
    my @source_state = (@_);

    foreach my $source_state (@source_state)
    {
        return 1 if $test_state eq $source_state;
    }

    return 0;
}

sub state_trans
{
    my @source_state = (@_);
    my $target_state = shift @source_state;

    print STDERR "Performing state transition $test_state => $target_state.\n"
        if $verbose >= 3;

    foreach my $source_state (@source_state)
    {
        if ($test_state eq $source_state)
        {
            $test_state = $target_state;
            return;
        }
    }

    print STDERR "ERROR: Unable to perform state transition ".
        "$test_state => $target_state\n" if $quiet == 0;
    $exitcode = 1;
    exit $exitcode;

}

# This kills test that has timed out
sub kill_test
{
    if (defined $nointeract) { print "Ending log analysis\n"; exit(1); }
    print STDERR "Killing test: $child_pid\n" if $verbose >= 3;
    if ($child_pid)
    {
        print STDERR "Killing test\n" if $verbose >= 3;
        my $res=kill 1, $child_pid;
        if ($res != 1)
        {
            my $err = $!;
            if (defined($err) && ($err ne 'No such process'))
            {
                print STDERR
                    "Unable to stop test [pid=$child_pid]: $err\n"
                    if $quiet == 0;
            }
        }
    }
    print STDERR "Closing monitoring handle\n" if $verbose >= 3;
    close(MONITORING_FILEHANDLE);
    print STDERR "Killed test\n" if $verbose >= 3;
}

# This continues test that has been killed/failed
sub continue_test
{
    if (defined $nointeract) { print "Ending log analysis\n"; exit(1); }
    my @cmdline;
    push @cmdline, "$exe";
    push @cmdline, "--suite=$current_suite" if defined $current_suite;
    push @cmdline, "--tcase=$current_tcase" if defined $current_tcase;
    if (defined $current_test)
    {
        if ($current_test =~ /^(.*):(\d+)$/)
        {
            push @cmdline, "--test=$1";
            push @cmdline, "--iter=$2";
        }
        else
        {
            push @cmdline, "--test=$current_test";
        }
    }
    push @cmdline, "--after";
    do_start_test(@cmdline, defined $interpreter);
}

# Finish test, cleanly (currently just calls kill_test)
sub bring_down_test
{
    if (defined $nointeract) { return; }
    $linebuf = ''; # Make sure no buffered input is left.
#    if ($endwait > 0 && (defined($_[0]) && $_[0] ne 'TIMEOUT')) #enforce waiting for endwait time
    if ($endwait > 0)
    {
        # While waiting, read out any logs potentially produced by test binary
        my $target_time = time() + $endwait;
        while(time() < $target_time and kill(0, $child_pid) == 1)
        {
            my $ignored_buf = '';
            my $s = IO::Select->new();
            $s->add(\*MONITORING_FILEHANDLE);
            my @ready = $s->can_read(1);
            if ($#ready >= 0)
            {
                my $res = sysread MONITORING_FILEHANDLE, $ignored_buf, 4096;
                write_outlog $ignored_buf if defined $res and $res > 0;
            }
        }
    }
    kill_test();
    $child_pid=0;
    state_trans('NOT_RUNNING',
                'SUITE_RUNNING', 'TCASE_RUNNING', 'TEST_RUNNING',
                'NOT_RUNNING');
}

# Handle events
sub event_suite_begin
{
    my $next_suite = $_[0];
    if (!defined $current_suite or
        $current_suite ne $next_suite)
    {
        print "Running test suite $next_suite:\n" if $quiet == 0;
        $current_suite = $next_suite;
        $current_tcase = undef;
        $current_test = undef;
    }
    $current_suite = $next_suite;
    # Currently at most single suite...
    state_trans('SUITE_RUNNING', 'NOT_RUNNING')
}

sub event_tcase_begin
{
    my $next_tcase = $_[0];
    if (!defined $current_tcase or
        $current_tcase ne $next_tcase)
    {
        print "  Running testcase $next_tcase:\n"
            if $quiet == 0;
        $current_tcase = $next_tcase;
        $current_test = undef;
    }
    state_trans('TCASE_RUNNING', 'SUITE_RUNNING', 'TCASE_RUNNING');
}

sub event_test_begin
{
    my $next_test = $_[0];
    my $next_test_txt = $next_test;
    $next_test_txt =~ s/^(.{55})....+/$1.../;
    printf "    Test %-58s ", $next_test_txt,
        if (!defined $current_test or
            $current_test ne $next_test) and
            $quiet == 0;
    $current_test = $next_test;
    state_trans('TEST_RUNNING', 'TCASE_RUNNING');
}

sub test_result
{
    if (exists $results{$_[0], $_[1], $_[2]})
    {
        print STDERR "Attempting to register result of test ".
            "$_[0]:$_[1]_:$_[2] twice.\n";
        $exitcode = 1;
        exit $exitcode;
    }
    $results{$_[0], $_[1], $_[2]} = $_[3];
    $results_comment{$_[0], $_[1], $_[2]} = $_[4];
    $results_tags{$_[0], $_[1], $_[2]} = $_[5] if defined $_[5];

    xmloutput_suite($_[0]);
    xmloutput_test(lc $_[3], defined($file_line)?$file_line:"unknown",
                   $_[2], "0", $_[1], $_[4], $_[5]);
    undef $file_line;
}

sub event_test_failure
{
    undef $measurement;
    undef $stackmeasurement;
    undef $heapmeasurement;
    test_result($current_suite, $current_tcase, $current_test,
                "FAILURE", $_[0]);

    print "FAIL!\n" if $quiet == 0;
    state_trans('TCASE_RUNNING', 'TEST_RUNNING');
    1;
}

sub event_test_unsupported
{
    undef $measurement;
    undef $stackmeasurement;
    undef $heapmeasurement;
    test_result($current_suite, $current_tcase, $current_test,
                "UNSUPPORTED", $_[0]);

    print "UNSUPPORTED\n" if $quiet == 0;
    state_trans('TCASE_RUNNING', 'TEST_RUNNING');
    1;
}

sub event_test_unsupported_quick
{
    test_result($current_suite, $current_tcase, $current_test,
                "UNSUPPORTED", $_[0]);

    print "UNSUPPORTED\n" if $quiet == 0;
    state_trans('TCASE_RUNNING', 'TEST_RUNNING');
    1;
}

sub event_test_success
{
    if (defined $measurement)
    {
        test_result($current_suite, $current_tcase, $current_test,
                    "SUCCESS", $_[0],
                    {'cycles' => $measurement->[0],
                     'nanos' => $measurement->[1]} );
    }
    else
    {
        if(defined $stackmeasurement)
        {
                test_result($current_suite, $current_tcase, $current_test,
                    "SUCCESS", $_[0],
                    {'StackMeasurement' => $stackmeasurement } );
        }
        elsif(defined $heapmeasurement)
        {
                test_result($current_suite, $current_tcase, $current_test,
                    "SUCCESS", $_[0],
                    {'HeapMeasurement' => $heapmeasurement } );
        }
        else
        {
                test_result($current_suite, $current_tcase, $current_test,
                    "SUCCESS", $_[0]);
        }
    }

    if (defined $measurement)
    {
        printf "%g\n", $measurement->[0];
        undef $measurement;
    }
    else
    {
        if (defined $stackmeasurement)
        {
                undef $stackmeasurement;
                print "SUCCESS\n" if $quiet == 0;
        }
        elsif (defined $heapmeasurement)
        {
                undef $heapmeasurement;
                print "SUCCESS\n" if $quiet == 0;
        }
        else
        {
                print "SUCCESS\n" if $quiet == 0;
        }
    }
    state_trans('TCASE_RUNNING', 'TEST_RUNNING');
    1;
}

sub event_test_error
{
    undef $measurement;
    test_result($current_suite, $current_tcase, $current_test,
                "ERROR", $_[0]);

    print "ERROR!\n" if $quiet == 0;
    state_trans('TCASE_RUNNING', 'TEST_RUNNING');
    1;
}

sub event_test_timeout
{
    undef $measurement;
    test_result($current_suite, $current_tcase, $current_test,
                "TIMEOUT", $_[0]);

    print "TIMEOUT!\n" if $quiet == 0;
    state_trans('NOT_RUNNING', 'TEST_RUNNING');
    1;
}

sub remember_line
{
    $file_line=$_[0];
    1;
}


sub getline
{
    my $res;
    # Handle lines remaining in receive buffer
  GETLINE_CHECK_HAVE_LINE:
    return $& if $linebuf =~ s/^[^\n]*\n//s;
    $res = sysread MONITORING_FILEHANDLE, $linebuf, 4096, length($linebuf);
    write_outlog substr($linebuf, -$res) if defined $res and $res > 0;
    if (defined $res && $res == 0 && $linebuf eq '')
    {
        # Signal EOF
        die "EOF\n";
    }
    elsif (defined $res && $res == 0)
    {
        my $result = $linebuf;
        $linebuf ='';
        return $result;
    }
    elsif (! defined $res)
    {
        die "$!\n";
    }
    goto GETLINE_CHECK_HAVE_LINE;
}


# Monitor test execution.
sub monitor_test
{
    # Monitoring loop
  LINE: for(;;)
    {
        my ($logtype, $event, $file, $func, $text);

        my $line;

        # Read line from monitoring filehandle, with optional timeout
        {
            my $s = IO::Select->new();
            $s->add(\*MONITORING_FILEHANDLE);

            print STDERR "*** Read input, timeout in $timeout secs\n"
                if $verbose >= 3;

            if ($linebuf =~ /\n/)
            {
                # Buffered lines exist, get one of them for handling.
                goto do_get_line;
            }

            #print STDERR "Waiting for can_read\n";
            my @ready = $s->can_read($timeout);
            #print STDERR "Signalled can_read (ready: @ready)\n";

            # Check for timeout
            if ($#ready < 0)
            {
                print STDERR "*** HANDLING TIMEOUT\n"
                    if $verbose >= 2;

                # In case of timeout ignore current linebuf
                # (which may contain partial line of text)
                $linebuf ='';

                return "TIMEOUT";
            }

            # Read complete line, we'll assume here only complete lines
            # are coming via log. Ie. if test crashes in middle of sending
            # a line, it might result in a hang.
          do_get_line:
            undef $!;
            eval {
                $line = getline();
            };
            return "EOF" if $@ eq "EOF\n";
            die "Read error: $@" if $@;
        }

        chomp $line;
        { local $/ = "\015"; chomp $line; }
        print STDERR "\[$line\]\n" if $verbose >= 2 && $line =~ /[^-]/;
        print STDERR "\[$line\]\n" if $verbose >= 5 && $line !~ /[^-]/;

        # Parse TESTLOG lines and ignore the rest.
        next LINE if $line !~
            m{^(LL_TESTLOG|LL_ASSERT),\s?(LF_\w+),\s?([^ :]+):(\d+):\s?(\w+):\s?(.*?)$};

        $logtype = $1; # always LL_TESTLOG or LL_ASSERT
        $event = $2;
        $file = $3;
        $line = $4;
        $func = $5;
        $text = $6;

        $file =~ s{^(./)?../../}{};

        print STDERR "Test output line: $event: $text\n"
            if $verbose >= 2;

        event_test_success($text) && return "CLEAN"
            if $expect_error{precondition} &&
            $logtype eq "LL_ASSERT" &&
            $event eq "LF_CONDITION" &&
            $text =~ /^precondition failed,/;

        event_test_success($text) && return "CLEAN"
            if $expect_error{postcondition} &&
            $logtype eq "LL_ASSERT" &&
            $event eq "LF_CONDITION" &&
            $text =~ /^postcondition failed,/;

        event_test_success($text) && return "CLEAN"
            if $expect_error{assert} &&
            $logtype eq "LL_ASSERT" &&
            $event eq "LF_ASSERT" &&
            $text =~ /^assertion failed,/;

        event_suite_begin($text) if $event eq "LF_SUITE_BEGIN";
        event_tcase_begin($text) if $event eq "LF_TESTCASE_START";
        event_tcase_begin($text) if $event eq "LF_TESTCASE_BEGIN";
        event_test_begin($text) if $event eq "LF_TEST_START";
        event_test_begin($text) if $event eq "LF_TEST_BEGIN";
        remember_line("$file:$line") if $event eq "LF_FAILURE";
        remember_line("$file:$line") if $event eq "LF_TESTFUNC_INVOKED";
        remember_line("$file:$line") if $event eq "LF_TESTFUNC_SUCCESS";
        remember_line("$file:$line") if $event eq "LF_TESTFUNC_UNSUPPORTED_QUICK";
        $result_unsupported_quick = 1 if $event eq "LF_TESTFUNC_UNSUPPORTED_QUICK";
        $expect_error{assert}=1 if $event eq
            "LF_TEST_EXPECT_ASSERT";
        $expect_error{precondition}=1 if $event eq
            "LF_TEST_EXPECT_PRECONDITION";
        $expect_error{postcondition}=1 if $event eq
            "LF_TEST_EXPECT_POSTCONDITION";
        event_test_unsupported($text) && return "UNSUPPORTED"
            if $event eq "LF_TEST_UNSUPPORTED";
        event_test_failure($text) && return "FAIL" if $event eq "LF_FAILURE";
        if (defined $result_unsupported_quick and $event eq "LF_TEST_END")
        {
            undef $result_unsupported_quick;
            event_test_unsupported_quick($text);
        }
        else
        {
            event_test_success($text) if $event eq "LF_TEST_END";
        }

        if ($event eq "LF_PERF_ACCEPTED_MEASUREMENT")
        {
            my ($number, $units) = split(/ /, $text);
            printf STDERR "Multiple measurements reported"
                if defined $measurement;
            exit 1 if defined $measurement;
            $measurement = scale_measurement($number, $units);
        }
        if ($event eq "LF_STACKUSAGE")
        {
            $stackmeasurement = $text;
        }
        if ($event eq "LF_HEAPUSAGE")
        {
            $heapmeasurement = $text;
        }
        return "FINISH" if $event eq "LF_GLOBAL_SUCCESS";
    }
}

# Signal handling, for Ctrl+C
sub catch_int
{
    # Interrupt test when keyboard interrupt arrives.
    kill_test() if defined $child_pid && $child_pid != 0;
    die "Terminated by Ctrl+C";
}
$::SIG{'INT'} = \&catch_int;

&xmlopen;
start_test();
 MAINLOOP: for(;;)
{
    my $status;
    $status = monitor_test();
    %expect_error = (); # Reset errors to expect
    print STDERR "monitor_test() returned $status.\n" if $verbose >= 3;
    if ($status eq "FAIL" || $status eq "UNSUPPORTED" || $status eq "CLEAN" ||
        ($status eq "EOF" && state_test('TEST_RUNNING')))
    {
        # Test binary ended due to test failure or test being unsupported,
        # or cleanup is just needed to continue at the next test.

        if ($status eq "EOF")
        {
            # Report failure
            event_test_error("Unexpected EOF")
        }

        bring_down_test();
        if (defined $requested_suite ||
            defined $requested_tcase ||
            defined $requested_test)
        {
            print STDERR
                "Failure (or expected failure/error) met and ".
                "required test/case/suite specified: bailing out.\n";
            last MAINLOOP;
        }
        continue_test();
        next MAINLOOP;
    }
    elsif ($status eq "TIMEOUT")
    {
        # Test binary ended due to test failure.
        # Continue at the next test.

        if (state_test('NOT_RUNNING'))
        {
            print STDERR "Global Timeout running tests.\n" if $quiet == 0;
            test_result("(undef)", "(undef)", "(undef)", "TIMEOUT",
                        "Global Timeout");
            last MAINLOOP;
        }
        elsif (state_test('SUITE_RUNNING'))
        {
            print STDERR "Suite '$current_suite' Timeout running tests.\n"
                if $quiet == 0;
            test_result($current_suite, "(undef)", "(undef)", "TIMEOUT",
                        "Suite Timeout");
            last MAINLOOP;
        }
        elsif (state_test('TCASE_RUNNING'))
        {
            print STDERR
                "Testcase '$current_tcase' Timeout running testcase.\n"
                if $quiet == 0;
            test_result($current_suite, $current_tcase, "(undef)", "TIMEOUT",
                        "Testcase Timeout");

            # Try next test case
            $current_test = undef;
        }
        elsif (state_test('TEST_RUNNING'))
        {
            # Timeout in specific test => try next test.
            event_test_timeout("Unexpected TIMEOUT");
        }
        else
        {
            print STDERR "ERROR: Invalid state: $test_state\n" if $quiet == 0;
            $exitcode = 1;
            exit $exitcode;
        }

        bring_down_test("TIMEOUT");
        if (defined $requested_suite ||
            defined $requested_tcase ||
            defined $requested_test)
        {
            print STDERR
                "Failure met and test/case/suite specified: bailing out.\n";
            last MAINLOOP;
        }
        continue_test();
        next MAINLOOP;
    }
    # Successful tests are handled internally by testrunner.pl.
    elsif ($status eq "EOF" && $test_state eq '')
    {
        # Test binary ended, assume fatal failure.
        bring_down_test();
        print STDERR "Test software exitted unexpectably.\n";
        $exitcode = 1;
        last MAINLOOP;
    }
    elsif ($status eq "FINISH")
    {
        # Test binary finished ok, assume end of tests.
        bring_down_test();
        $exitcode = 0;
        last MAINLOOP;
    }

    # Unhandled branch.
    bring_down_test();
    die "Unexpected monitor status: $status\n";
}

my @keys = keys %results;

my $successful_count = 0;
my $failure_count = 0;
my $error_count = 0;
my $timeout_count = 0;
my $unsupported_count = 0;
my $total_count = 0;

foreach my $key (@keys)
{
    ++$successful_count && next if $results{$key} eq 'SUCCESS';
    ++$unsupported_count && next if $results{$key} eq 'UNSUPPORTED';
    ++$failure_count && next if $results{$key} eq 'FAILURE';
    ++$timeout_count && next if $results{$key} eq 'TIMEOUT';
    ++$error_count; # Count also "miscategorized entries" (not O/F/T) as errors
}

$total_count = $#keys + 1 - $unsupported_count;

printf
    "RESULTS: SUCCESS: %d FAILED: %d ERROR: %d TIMED OUT: %d (TOTAL: %d)\n",
    $successful_count, $failure_count, $error_count, $timeout_count,
    $total_count if $quiet <= 1;

printf "UNSUPPORTED TESTS: %d unsupported tests were ignored\n",
    $unsupported_count if $unsupported_count;

# Assume success if no $exitcode set.
$exitcode = 0 if !defined $exitcode;

# Force error exit if some tests failed.
$exitcode = 1 if (($successful_count+$unsupported_count) <= $#keys) and
                 $exitcode== 0;

# Exit with given exit code.
exit $exitcode;

__END__

=head1 NAME

testrunner.pl - Runs executes unit tests and monitors their execution

=head1 SYNOPSIS

testrunner.pl [options] test-executable

    Options:
      --help             brief help message
      --man              full documentation
      --suite=SUITE      run only specified test suite
      --tcase=TCASE      run only specified testcase
      --test=TEST        run only specified test
      --timeout=SECS     timeout for execution of any single test (default 60)
      --end-wait=SECS    give test binary some time to finish (default 5)
      --xmllog=FILE      make XML log file of test run
      --interpreter=EXE  binary that needs to be used to execute test binaries
      --interpreter-opts=OPTIONS  extra options to interpreter
      --frequency=FREQ   Provide processor frequency for performance tests
      --ld-preload=SO    specify dynamic libraries to preload
      --use-argfile      pass arguments to test binaries via input files
      --verbose          make testrunner.pl more verbose
      --quiet            omit even part of standard testrunner.pl printouts
      --no-interact      run non-interactively, test-executable is
                         log of preceding test run

=head1 OPTIONS

=over 8

=item B<--help>

Print a brief help message and exit.

=item B<--man>

Prints the manual page and exits.

=item B<--suite=SUITE>

Only execute specified test suite.
(Currently test binaries typically contain only single test suite so this
option might not be all that useful.)

=item B<--tcase=TCASE>

Only execute specified test case.

=item B<--test=TEST>

Only execute specified test.
Notice: you may need to also use B<--tcase> if you have tests with the same name
in different test suites.

=item B<--timeout=SECS>

Timeout in seconds for each test. If single test executes longer than this,
the test is assumed to be in infinite loop and test execution is forcibly
ended and the test is assumed to have timed out.
On emulated targets, it is likely that much higher timeouts are required
than default of 60 seconds.

=item B<--xmllog=FILE>

Produce log of execution in XML format to specified file.
The produced XML log file aims compatibility with check.

=item B<--outlog=FILE>

testrunner.pl scans the logs produced by tested binary and produces
its conclusions based on that data. For debugging the applications or
testrunner.pl itself, it is often desirable to store all output generated
by tested application. This command line option provides that.

You may use >> and | in output file name as described in perlopentut.

=item B<--interpreter=EXE>

For non-native binaries, this command line parameter can provide interpreter
that executes the binary. For example, for ARM binaries using EABI, you might
want to use arm-none-eabi-run.

=item B<--interpreter-opts=OPTIONS>

For non-native binaries, this command line parameter can provide interpreter
that executes the binary. For example, for ARM binaries using EABI, you might
want to use arm-none-eabi-run. This parameters allows to pass
additional command line options to interpreter.

=item B<--frequency=FREQ>

Provide processor frequency for performance tests. You need to provide this
parameter if you are running performance tests on remote host or
if testrunner.pl is unable to determine CPU frequency running on this host.

=item B<--ld-preload=SO>

This option allows to specify dynamic objects to preload for specified
binary or interpreter. Most common usage is to request interpreters, such
as arm-none-eabi-run to unbuffer their stdout via --ld-preload=nobuffer.so.

=item B<--use-argfile>

Instead of int argc, char *argv[] parameter passing used for unix command line,
a file that contains arguments for the program is created. This convention
allows some semi-hosting emulators, such as arm-none-eabi-run to get arguments
for test binary.

=item B<--verbose>

Make testrunner.pl output more debugging information.
Repeat B<--verbose> to get even more.

=item B<--quiet>

Make testrunner.pl output less information than it typically does.
Repeat B<--quiet> to get even less.

=back

=head1 DESCRIPTION

B<This program> executes test executable (typically single test suite) given
on command line and executes all tests within the suite and monitors output
of executing tests for timeout and/or failed tests.
After timeout and/or failed test, tests shall restart at test immediately
preceding timeouted or failed test.

=head1 BUGS

B<timeout> option sets timeout for any log messages during running test.
If running test is caught in infinite loop that produces log messages all the
time, the test shall never timeout.

=cut

