#################################################################
#
#  verInc.pl - sript for FW version increment on each build
#  usage: verInc.pl <WORKSPACE_ROOT> <BUILD_TYPE(optional)>
##################################################################

my $workspaceRoot=$ARGV[0];
my $build_type=$ARGV[1];
#Look for version file in <WORKSPACE_ROOT>/version
my $versionFile=$workspaceRoot.'/version/version.h';
my $major, $minor, $build, $sub_build = 0;

open FILE, "$versionFile" or die "$!";
my @lines = <FILE>;
close FILE or die "$!";;

open FILE, ">$versionFile" or die "$!";

foreach my $line (@lines) 
{ 
	if ( $line =~ /^.+MAJOR_VERSION[.\D]+(\d+)\"*/ )
	{
		$major = $1;
	}
	elsif ( $line =~ /^.+MINOR_VERSION[.\D]+(\d+)\"*/ )
	{
		$minor = $1;
	}
	elsif ( $line =~ /^.+ BUILD_VERSION[.\D]+(\d+)\"*/ )
	# BUILD_VERSION  - increment for each daily (non-release) build
	{
		my ($old_build) = $1;
		$build = $old_build;
		if ( $build_type ne "release" )
		{
			$build = $old_build+1;
			$line =~ s/$old_build/$build/ ;
		}
	}
	elsif ( $line =~ /^.+ SUB_BUILD_VERSION[.\D]+(\d+)\"*/ )
	# SUB_BUILD_VERSION  - increment for each release build
	{
		my ($old_sub_build) = $1;
		$sub_build = $old_sub_build;
		if ( $build_type eq "release" )
		{
			$sub_build = $old_sub_build+1;
			$line =~ s/$old_sub_build/$sub_build/ ;
		}
	}
	
	print FILE $line or die "$!";
}
close FILE or die "$!";
print "VERSION_NUM=$major.$minor.$build.$sub_build\n";
