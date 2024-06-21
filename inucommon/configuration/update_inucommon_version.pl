
my ($workDir, $version) = @ARGV;

my ($vMajor,$vMinor,$vRelease,$vBuild) = split('\.', $version);

print $vMajor.$vMinor.$vRelease.$vBuild;

my $InuCommonVersionFile = $workDir."/include/Version.h";


open (FILE, $InuCommonVersionFile) or die "couldn't open $InuCommonVersionFile - $!";

my @lines = <FILE>;

close FILE;

open (FILE, ">$InuCommonVersionFile") or die "couldn't open $InuCommonVersionFile for writing - $!"; 

foreach my $line (@lines)
{
   $line =~ s/(\#define INUCOMMON_VERSION_1[^\d]+)\d+(.*)/$1$vMajor$2/;
   $line =~ s/(\#define INUCOMMON_VERSION_2[^\d]+)\d+(.*)/$1$vMinor$2/;
   $line =~ s/(\#define INUCOMMON_VERSION_3[^\d]+)\d+(.*)/$1$vRelease$2/;
   $line =~ s/(\#define INUCOMMON_VERSION_4[^\d]+)\d+(.*)/$1$vBuild$2/;
   $line =~ s/\#define INUCOMMON_VERSION_STR.+/#define INUCOMMON_VERSION_STR "$vMajor.$vMinor.$vRelease.$vBuild"/;
   printf FILE $line;
}

close FILE;

