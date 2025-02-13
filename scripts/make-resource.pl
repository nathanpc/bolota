#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use Data::Dumper;
use Cwd qw(abs_path);
use File::Basename qw(dirname basename);

# Directories
use constant PROJECT_ROOT => dirname(dirname(abs_path($0)));
use constant TEMPLATE_DIR => PROJECT_ROOT . '/res/templates';
use constant SNIPPET_DIR  => PROJECT_ROOT . '/res/snippets';

# Substitutes variables in a string.
sub substitute_variables {
	my ($vars, $str, $opts) = @_;
	
	# Check if we actually have a variable to replace.
	while ($str =~ m/%(?<varname>[\w]+)%/) {
		# Check if the variable is actually defined.
		my $varname = $+{'varname'};
		if (!exists($vars->{$varname})) {
			print "ERROR: Variable \"$varname\" not found.\n";
			return $str;
		}
		
		# Substitute the variable.
		$str =~ s/%\Q$varname\E%/$vars->{$varname}/g;
	}
	
	# Check if we have any special needs.
	if (defined $opts) {
		# Handle Windows Mobile specific stuff.
		if (exists $opts->{'windows_mobile'}) {
			# Remove & from PocketPC top menus.
			if ($str =~ m/[\t ]+POPUP "[\w\d&]+"/) {
				$str =~ s/&//;
			}
			
			# Remove shortcuts from menu items.
			if ($str =~ m/([\t ]+MENUITEM "[^\\"]+)([^"]+)",(.+)/) {
				$str = $1 . '",' . (' ' x length($2)) . $3;
			}
		}
	}
	
	return $str;
}

# Reads a snippet file and returns a line by line array of it.
sub read_include {
	my ($fname) = @_;
	
	# Read the file.
	my @lines = ();
	open(my $fh, '+<:encoding(UTF-8)', SNIPPET_DIR . "/$fname");
	while (my $line = <$fh>) {
		push @lines, $line;
	}
	close($fh);
	
	return @lines;
}

# Generates a final resource file as a line array.
sub generate_rc {
	my ($fpath) = @_;
	
	# Read file and substitute include directives.
	my @lines = ();
	print '    Reading ' . basename($fpath) . " template...\n";
	open(my $fh, '+<:encoding(UTF-8)', $fpath);
	while (my $line = <$fh>) {
		if ($line =~ m{///\s+\.inc\s+([A-Za-z0-9\.\-_/]+)}) {
			# Include directive.
			print "    Including $1... ";
			push @lines, read_include($1);
			print "ok\n";
		} else {
			# Regular line.
			push @lines, $line;
		}
	}
	close($fh);
	
	return @lines;
}

# Renders a template to its final destination.
sub render_template {
	my ($label, $tpname, $outfile, $vars, $opts) = @_;
	
	# Generate the rendered output lines.
	print "Generating $label resource file:\n";
	my @lines = generate_rc(TEMPLATE_DIR . "/$tpname");
	
	# Write lines to the output file.
	print '    Writing resource file... ';
	open(my $fh, '>:encoding(UTF-8)', $outfile);
	print $fh substitute_variables($vars, $_, $opts) for @lines;
	close($fh);
	print "ok\n";
}

# Script's main entry point.
sub main {
	# Define variables.
	my $vars = {
		'font' => '"MS Shell Dlg", 400, 0, 0x1'
	};

	# Render desktop templates.
	render_template('Visual Studio 2022', 'vs2012.rc', PROJECT_ROOT .
		"/vs2022/Bolota.rc", $vars);
	render_template('Visual Studio 2012', 'vs2012.rc', PROJECT_ROOT .
		"/vs2012/Bolota.rc", $vars);
	render_template('Visual C++ 6', 'vc6.rc', PROJECT_ROOT .
		"/vc6/Bolota.rc", $vars);
		
	# Windows CE templates.
	$vars->{'font'} = '"System"';
	render_template('eMbedded Visual C++ 3 (WinCE)', 'wince.rc', PROJECT_ROOT .
		"/wince/Bolota.rc", $vars);
	render_template('eMbedded Visual C++ 3 (PocketPC)', 'wince.rc', PROJECT_ROOT .
		"/pocketpc/Bolota.rc", $vars, { 'windows_mobile' => 1 });
	render_template('Visual Studio 2005 (PocketPC)', 'wmppc.rc', PROJECT_ROOT .
		"/winmobile/Bolotappc.rc", $vars, { 'windows_mobile' => 1 });
	render_template('Visual Studio 2005 (Smartphone)', 'wmsp.rc', PROJECT_ROOT .
		"/winmobile/Bolotasp.rc", $vars, { 'windows_mobile' => 1 });
}

# Execute the script.
main();
