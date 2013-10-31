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

sub get_host_tools() {
	my @hosttools;

	printf("Get host tools list...");
	@hosttools = split / /,`make --no-print-directory val.tools-y V=s 2>/dev/null`;
	printf("done\n");

	chomp @hosttools;
	@hosttools = grep { ! /^$/ } @hosttools;

	foreach my $pkg (sort @hosttools) {
		printf("\033[M\r");
		printf("Get host src pkg name...$pkg");
		my $pkgname = `make -C tools/${pkg} val.PKG_SOURCE --no-print-directory TOPDIR=\`pwd\` 2>/dev/null`;
		chomp $pkgname;
		-f "dl/$pkgname" and
			$localpkgs{$pkgname}->{host} = "x" unless $pkgname =~ m/^$/;
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
		-f "dl/$pkgname" and
			$localpkgs{$pkgname}->{target} = "x" unless $pkgname =~ m/^$/;
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
		printf("\033[M\r");
		printf("Get toolchain pkg name...");
		my $pkgname = `make -C toolchain/${pkg} val.PKG_SOURCE --no-print-directory TOPDIR=\`pwd\` 2>/dev/null`;
		chomp $pkgname;
		-f "dl/$pkgname" and
			$localpkgs{$pkgname}->{toolchain} = "x" unless $pkgname =~ m/^$/;
	}
	printf("\033[M\r");
	printf("Get toolchain pkg name...done\n");
}

sub get_linux_pkg() {
	my $targetdir = "target/linux";

	opendir(DIR, $targetdir) or die("\"$targetdir\" does not exist");
	foreach my $target (readdir(DIR)) {
		next if ! -d "$targetdir/$target";
		printf("\033[M\r");
		printf("Get kernel pkg name...$target");
		my $pkgname = `make -C $targetdir/$target val.LINUX_SOURCE --no-print-directory TOPDIR=\`pwd\` 2>/dev/null`;
		chomp $pkgname;
		if (-f "dl/$pkgname") {
			$localpkgs{$pkgname}->{target} = "x";
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
		printf("%-70s\t%-8s\t%-12s\t%-12s\n",
			$src,
			$localpkgs{$src}->{host},
			$localpkgs{$src}->{toolchain},
			$localpkgs{$src}->{target});
	}
}

sub write_xlsx() {
	my $workbook = Excel::Writer::XLSX->new($opts{o});
	if (!defined($workbook)) {
		die "Error: Unable to open \"$opts{o}\"";
	}

	my $worksheet = $workbook->add_worksheet();
	$worksheet->set_column("A:A", 80);
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

	foreach my $src (sort keys %localpkgs) {
		$row++;
		$worksheet->write($row, 0, $src);
		$worksheet->write($row, 1, $localpkgs{$src}->{host}, $select_format)
				if exists($localpkgs{$src}->{host});
		$worksheet->write($row, 2, $localpkgs{$src}->{toolchain}, $select_format)
				if exists($localpkgs{$src}->{toolchain});
		$worksheet->write($row, 3, $localpkgs{$src}->{target}, $select_format)
				if exists($localpkgs{$src}->{target});
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
