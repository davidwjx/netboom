my ($workDir, $version) = @ARGV;

my ($vMajor,$vMinor,$vRelease,$vBuild) = split('\.', $version);

print $vMajor.$vMinor.$vRelease.$vBuild;

my $FWVersionFile = $workDir."/Version.h";
my $FWCommonVersionFile = $workDir."/FW/common/version/version.h";

open (FILE, $FWVersionFile) or die "couldn't open $FWVersionFile - $!";

my @lines = <FILE>;

close FILE;

open (FILE, ">$FWVersionFile") or die "couldn't open $FWVersionFile for writing - $!"; 

foreach my $line (@lines)
{
   $line =~ s/(\#define CM_VERSION_1[^\d]+)\d+(.*)/$1$vMajor$2/;
   $line =~ s/(\#define CM_VERSION_2[^\d]+)\d+(.*)/$1$vMinor$2/;
   $line =~ s/(\#define CM_VERSION_3[^\d]+)\d+(.*)/$1$vRelease$2/;
   $line =~ s/(\#define CM_VERSION_4[^\d]+)\d+(.*)/$1$vBuild$2/;
   $line =~ s/\#define CM_VERSION_STR.+/#define CM_VERSION_STR "$vMajor.$vMinor.$vRelease.$vBuild"/;
   printf FILE $line;
}

close FILE;

open (FILE, $FWCommonVersionFile) or die "couldn't open $FWCommonVersionFile - $!";

my @lines = <FILE>;

close FILE;

my $vRelease = substr($vRelease, 2, 3);

open (FILE, ">$FWCommonVersionFile") or die "couldn't open $FWCommonVersionFile for writing - $!"; 

foreach my $line (@lines)
{
   $line =~ s/(\#define MAJOR_VERSION[^\d]+)\d+(.*)/$1$vMajor$2/;
   $line =~ s/(\#define MINOR_VERSION[^\d]+)\d+(.*)/$1$vMinor$2/;
   $line =~ s/(\#define BUILD_VERSION[^\d]+)\d+(.*)/$1$vRelease$2/;
   $line =~ s/(\#define SUB_BUILD_VERSION[^\d]+)\d+(.*)/$1$vBuild$2/;
   printf FILE $line;
}

close FILE;

$vMinor =~ s/^0+(?=[0-9])//;
if ($vRelease ne "00"){
   $vRelease =~ s/^0+(?=[0-9])//;
}
$vBuild =~ s/^0+(?=[0-9])//;

open (FILE, ">$FWCommonVersionFile") or die "couldn't open $FWCommonVersionFile for writing - $!"; 

foreach my $line (@lines)
{
   $line =~ s/(\#define MAJOR_VERSION [^\d]+)\d+(.*)/$1$vMajor$2/;
   $line =~ s/(\#define MINOR_VERSION [^\d]+)\d+(.*)/$1$vMinor$2/;
   $line =~ s/(\#define BUILD_VERSION [^\d]+)\d+(.*)/$1$vRelease$2/;
   $line =~ s/(\#define SUB_BUILD_VERSION [^\d]+)\d+(.*)/$1$vBuild$2/;
   printf FILE $line;
}

close FILE;
