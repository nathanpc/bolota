#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use Data::Dumper;
use Cwd qw(abs_path);
use File::Basename qw(dirname basename);

# Directories
use constant PROJECT_ROOT => dirname(dirname(abs_path($0)));
use constant ICONS_DIR => PROJECT_ROOT . '/icons/fields';

# Replaces the section of the file responsible for holding the field icons.
sub replace_section {
	my ($fpath, $lines) = @_;
	
	my $pos = -1;
	my @before = ();
	my @after = ();
	
	# Read in the file and isolate our section.
	open(my $fh, '+<:encoding(UTF-8)', $fpath);
	while (my $line = <$fh>) {
		if ($pos < 0) {
			# Before field icons section.
			push @before, $line;
			if ($line =~ m/\/\/ Begin field icons/) {
				$pos = 0;
			}
		} elsif ($pos == 0) {
			# Inside definitions.
			if ($line =~ m/\/\/ End of field icons/) {
				push @after, $line;
				$pos = 1;
			}
		} else {
			# After field icons section.
			push @after, $line;
		}
	}
	close($fh);
	
	# Check if we actually had a marker.
	if (scalar(@after) == 0) {
		die "No field icons section marker was found in file $fpath";
	}
	
	# Write the new file.
	open($fh, '>:encoding(UTF-8)', $fpath);
	print $fh $_ for @before;
	print $fh $_ for @$lines;
	print $fh $_ for @after;
	close($fh);
	
	print "ok\n";
}

# Updates the resources definitions.
sub update_resources {
	my ($bullets, $icons) = @_;

	# Make section lines.
	print "Updating icons resource snippet file... ";
	my @lines = ();
	for my $icon (@$bullets) {
		push @lines, sprintf('IDI_FI%-17s ICON  DISCARDABLE       ' .
			'"../icons/fields/%s.ico"', uc($icon), $icon) . "\r\n";
	}
	for my $icon (@$icons) {
		push @lines, sprintf('IDI_FI_%-16s ICON  DISCARDABLE       ' .
			'"../icons/fields/%s.ico"', uc($icon), $icon) . "\r\n";
	}

	# Replace the field icons section in the resources file.
	my $fpath = PROJECT_ROOT . "/res/snippets/icons.rc";
	replace_section($fpath, \@lines);
}

# Updates the shared resource ID definitions.
sub update_shared_ids {
	my ($base, $bullets, $icons) = @_;

	# Make section lines.
	print 'Updating shared resources header... ';
	my $id = $base - 1;
	my @lines = ();
	for my $icon (@$bullets) {
		push @lines, sprintf('#define IDI_FI%-25s %d', uc($icon), $id--) .
			"\r\n";
	}
	$id = $base + 1;
	for my $icon (@$icons) {
		push @lines, sprintf('#define IDI_FI_%-24s %d', uc($icon), $id++) .
			"\r\n";
	}

	# Replace the field icons section in the shared resources file.
	my $fpath = PROJECT_ROOT . "/src/SharedResources.h";
	replace_section($fpath, \@lines);
}

# Updates the FieldImageList.cpp definitions.
sub update_imagelist {
	my ($icons) = @_;

	# Make section lines.
	print 'Updating FieldImageList... ';
	my @lines = ();
	for my $icon (@$icons) {
		# Check if we need to add a prefix.
		my $prefix = '';
		if ($icon eq 'Calendar') {
			$prefix = 'm_usIndexCalendar = ';
		}
		
		# Build up the AddIcon definition.
		push @lines, "\t$prefix" . sprintf('AddIcon(_T("%s"), IDI_FI_%s);',
			$icon, uc($icon)) . "\r\n";
	}

	# Replace the field icons section in the file.
	my $fpath = PROJECT_ROOT . "/src/Components/FieldImageList.cpp";
	replace_section($fpath, \@lines);
}

# Updates the IconField.h definitions.
sub update_icon_field_enum {
	my ($icons) = @_;
	
	# Get the size of the longest label.
	my $len = 0;
	for my $icon (@$icons) {
		if (length($icon) > $len) {
			$len = length($icon);
		}
	}

	# Make section lines.
	print 'Updating IconField.h enum... ';
	my @lines = ();
	my $index = 1;
	push @lines, sprintf("\tBOLOTA_ICON_%-" . $len . "s = 0,\r\n", 'NONE');
	for my $icon (@$icons) {
		# Build up the enum item.
		push @lines, "\t" . sprintf('BOLOTA_ICON_%-' . $len . 's = %d,',
			uc($icon), $index++) . "\r\n";
	}
	
	# Replace field icons count.
	my $fpath = PROJECT_ROOT . "/src/Bolota/IconField.h";
	rename($fpath, "$fpath.bak");
	open(my $in, '<:encoding(UTF-8)', "$fpath.bak");
	open(my $out, '>:encoding(UTF-8)', $fpath);
	while (my $line = <$in>) {
		if ($line =~ m/\#define\s+BOLOTA_FIELD_ICON_NUM/) {
			print $out '#define BOLOTA_FIELD_ICON_NUM ' . scalar(@$icons) .
				"\r\n";
			next;
		}
		
		print $out $line;
	}
	close($in);
	close($out);
	unlink("$fpath.bak");

	# Replace the field icons section in the file.
	replace_section($fpath, \@lines);
}

# Updates the IconField.cpp icons list.
sub update_icon_field_list {
	my ($icons) = @_;
	
	# Make section lines.
	print 'Updating IconField.cpp icon list... ';
	my @lines = ();
	my @iconlist = @$icons;
	for my $i (0..$#iconlist) {
		push @lines, "\tBOLOTA_ICON_" . uc($iconlist[$i]) .
			(($i == $#iconlist) ? '' : ',') . "\r\n";
	}
	
	# Replace the field icons section in the file.
	my $fpath = PROJECT_ROOT . "/src/Bolota/IconField.cpp";
	replace_section($fpath, \@lines);
}

# Script's main entry point.
sub main {
	my @bullets = ();
	my @icons = ();
	
	# Get field icons from folder.
	for my $icon (glob(ICONS_DIR . '/*.ico')) {
		# Icons that start with an underscore are special.
		if (basename($icon) =~ m/^_/) {
			push @bullets, basename($icon) =~ s/\..+$//r;
			next;
		}
		
		# Regular field icon.
		push @icons, basename($icon) =~ s/\..+$//r;
	}
	
	# Update files.
	update_resources(\@bullets, \@icons);
	update_shared_ids(3000, \@bullets, \@icons);
	update_imagelist(\@icons);
	update_icon_field_enum(\@icons);
	update_icon_field_list(\@icons);
}

# Execute the script.
main();
