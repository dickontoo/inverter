#!/usr/bin/perl
#
# log.pl
#
# (C) 2015 Dickon Hood <dickon@fluff.org>
#
# Actually log the output of the inverter.
#
# DB created by:
#
# rrdtool create inverter.rrd --step 300 -b 1448064000 \
#	DS:temp:GAUGE:600:0:100 \
#	DS:string1v:GAUGE:600:0:400 \
#	DS:string2v:GAUGE:600:0:400 \
#	DS:string1i:GAUGE:600:0:12 \
#	DS:string2i:GAUGE:600:0:12 \
#	DS:acv:GAUGE:600:215:256 \
#	DS:acf:GAUGE:600:48:52 \
#	DS:acp:GAUGE:600:0:4000 \
#	DS:aci:GAUGE:600:0:20 \
#	RRA:AVERAGE:0.5:1:946728000
# 

use strict;
use HTTP::Date;
use Data::Dumper;

my $dpath = '/var/local/rrd/inverter.rrd';
my %h;

while (1) {
	%h = ();
	while (1) {
		my $l = <STDIN>;
		last if ($l eq "\n");
		$l =~ /^([^:]+)\:\s+([^\s]+)/;
		$h{$1} = $2;
	}
	dolog(\%h);
	exit 0 if eof STDIN;
}


sub dolog
{
	my $h = shift;
#	print Dumper($h);

	my $t = str2time($$h{'Time'});

	my $e = "rrdtool update $dpath $t:$$h{'Temperature'}:$$h{'PV1 voltage'}:$$h{'PV2 voltage'}:$$h{'PV1 current'}:$$h{'PV2 current'}:$$h{'L1 voltage'}:$$h{'Frequency'}:$$h{'L1 power'}:$$h{'L1 current'}";
	print $e."\n";
	system $e;
}

