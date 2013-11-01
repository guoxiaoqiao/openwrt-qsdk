#!/usr/bin/env perl
use FindBin;
use lib "$FindBin::Bin";
use strict;
use metadata;
use Getopt::Std;
use Excel::Writer::XLSX;

use Data::Dumper;

my %opts;
my %localpkgs;

$Getopt::Std::STANDARD_HELP_VERSION = 1;

sub HELP_MESSAGE {
	print "$ARGV[0] [-o file.xlsx]\n";
}

sub get_info_from_make($$) {
	my $ret=shift;
	my $dir=shift;

	my @pkgname = `make -C ${dir} var.PKG_SOURCE var.PKG_VERSION var.LINUX_SOURCE var.LINUX_VERSION --no-print-directory TOPDIR=\`pwd\` 2>/dev/null`;
	chomp @pkgname;

	return if ($#pkgname==-1);

	foreach (@pkgname) {
		(my $name, my $value) = ($_ =~ m/(.*)=\'(.*)\'/);
		$ret->{$name}=$value;
	}
}

sub get_host_tools() {
	my @hosttools;

	printf("Get host tools list...");
	@hosttools = split / /,`make --no-print-directory val.tools-y V=s 2>/dev/null`;
	printf("done\n");

	chomp @hosttools;
	@hosttools = grep { ! /^$/ } @hosttools;

	foreach my $pkg (sort @hosttools) {
		my %h_pkg;

		printf("\033[M\r");
		printf("Get host src pkg name...$pkg");
		get_info_from_make(\%h_pkg, "tools/$pkg");

		if (-f "dl/$h_pkg{PKG_SOURCE}") {
			$localpkgs{$h_pkg{PKG_SOURCE}}->{host} = "x"
				unless $h_pkg{PKG_SOURCE} =~ m/^$/;
			$localpkgs{$h_pkg{PKG_SOURCE}}->{version} = $h_pkg{PKG_VERSION};
		}
	}
	printf("\033[M\r");
	printf("Get host src pkg name...done\n");
}

sub get_target_pkgs() {
	printf("Get target pkg name...");
	parse_package_metadata("tmp/.packageinfo") or exit 1;

	foreach my $pkg (sort {uc($a) cmp uc($b)} keys %package) {
		my $pkgname = $package{$pkg}->{source};
		printf("\033[M\r");
		printf("Get target pkg name...$pkg");
		if (-f "dl/$pkgname") {
			$localpkgs{$pkgname}->{target} = "x" unless $pkgname =~ m/^$/;
			$localpkgs{$pkgname}->{version} = $package{$pkg}->{version};
		}
	}

	printf("\033[M\r");
	printf("Get target pkg name...done\n");
}

sub get_toolchain_pkgs() {
	my @toolchaindirs = (
			"binutils",
			"eglibc",
			"gcc",
			"gdb",
			"kernel-headers",
			"llvm",
			"uClibc",
			"wrapper",
		);
	foreach my $pkg (sort @toolchaindirs) {
		my %h_pkg;

		printf("\033[M\r");
		printf("Get toolchain pkg name...");
		get_info_from_make(\%h_pkg, "toolchain/$pkg");

		if (-f "dl/$h_pkg{PKG_SOURCE}") {
			$localpkgs{$h_pkg{PKG_SOURCE}}->{toolchain} = "x" unless $h_pkg{PKG_SOURCE} =~ m/^$/;
			$localpkgs{$h_pkg{PKG_SOURCE}}->{version} = $h_pkg{PKG_VERSION};
		}
	}
	printf("\033[M\r");
	printf("Get toolchain pkg name...done\n");
}

sub get_linux_pkg() {
	my $targetdir = "target/linux";

	opendir(DIR, $targetdir) or die("\"$targetdir\" does not exist");
	foreach my $target (readdir(DIR)) {
		my %h_pkg;

		next if ! -d "$targetdir/$target";
		printf("\033[M\r");
		printf("Get kernel pkg name...$target");
		get_info_from_make(\%h_pkg, "$targetdir/$target");
		if (-f "dl/$h_pkg{LINUX_SOURCE}") {
			$localpkgs{$h_pkg{LINUX_SOURCE}}->{target} = "x";
			$localpkgs{$h_pkg{LINUX_VERSION}}->{version} = $h_pkg{LINUX_VERSION};
		}
	}
	printf("\033[M\r");
	printf("Get kernel pkg name...done\n");
	closedir(DIR);
}

sub is_downloaded($) {
	my $source = shift;
	-f "dl/$source" and return 1;
	return 0;
}

sub write_txt() {
	printf("%-70s\t%-8s\t%-12s\t%-12s\n",
		"source","host","toolchain","target");
	foreach my $src (sort keys %localpkgs) {
		# We fill in the hash with empty strings to facilitate the print below
		$localpkgs{$src}->{host} = "" unless exists($localpkgs{$src}->{host});
		$localpkgs{$src}->{toolchain} = "" unless exists($localpkgs{$src}->{toolchain});
		$localpkgs{$src}->{target} = "" unless exists($localpkgs{$src}->{target});
		printf("%-70s\t%-8s\t%-12s\t%-12s\t%-20s\n",
			$src,
			$localpkgs{$src}->{host},
			$localpkgs{$src}->{toolchain},
			$localpkgs{$src}->{target},
			$localpkgs{$src}->{version});
	}
}

sub write_xlsx() {
	my $workbook = Excel::Writer::XLSX->new($opts{o});
	if (!defined($workbook)) {
		die "Error: Unable to open \"$opts{o}\"";
	}

	my $worksheet = $workbook->add_worksheet();
	$worksheet->set_column("A:A", 80);
	$worksheet->set_column("E:E", 30);
	$worksheet->autofilter("B:D");

	my $title_format = $workbook->add_format();
	$title_format->set_bold();
	$title_format->set_align("center");

	my $select_format = $workbook->add_format();
	$select_format->set_align("center");

	my $row = 0;
	$worksheet->write($row, 0, "source", $title_format);
	$worksheet->write($row, 1, "host", $title_format);
	$worksheet->write($row, 2, "toolchain", $title_format);
	$worksheet->write($row, 3, "target", $title_format);
	$worksheet->write($row, 4, "version", $title_format);

	foreach my $src (sort keys %localpkgs) {
		$row++;
		$worksheet->write($row, 0, $src);
		$worksheet->write($row, 1, $localpkgs{$src}->{host}, $select_format)
				if exists($localpkgs{$src}->{host});
		$worksheet->write($row, 2, $localpkgs{$src}->{toolchain}, $select_format)
				if exists($localpkgs{$src}->{toolchain});
		$worksheet->write($row, 3, $localpkgs{$src}->{target}, $select_format)
				if exists($localpkgs{$src}->{target});
		$worksheet->write_string($row, 4, $localpkgs{$src}->{version})
				if exists($localpkgs{$src}->{version});
	}
}

sub main() {
	get_host_tools();
	get_target_pkgs();
	get_toolchain_pkgs();
	get_linux_pkg();

	write_txt();
	write_xlsx() if (exists($opts{o}) && defined($opts{o}));
}

getopts('o:', \%opts);
main();
