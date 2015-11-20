inverter
========

(C) 2015 Dickon Hood <dickon@fluff.org>

inverter is a trivial program to take the output of a SolaX (pron. 'Solar
Ex', somewhat annoyingly) SL-TL 3600T solar PV inverter, and dump some data
from it.  It may work on other, similar devices; Suntellite and Solar River
appear in the Google searches I've done.

As the amount of documentation on Internet about this datalogger is nill or
less, I've had to reverse-engineer the protocol it uses.  Happily, this is
extremely simple in the normal case, as a quick perusal of the code will
show.

It's intended to be a stage in a pipeline, consisting of a socket handler,
it, and a grapher.  It makes no attempt to log anything itself.

The SolaX inverters consist of two parts: the inverter, and a datalogger.
It's the datalogger which spits out data, and has some limited
configurability.

Don't expect to get anything out of it at night: the inverter is
solar-powered, and takes nothing out of the grid.  When it gets dark and the
panels are unable to power the thing, it simply shuts down.

The docs say it reports every 30 seconds.  This is a lie.  It's every 300s
or so.  There is a 'server' mode (rather than client, which is the default)
where it's listening on port 8899, but nothing I've sent it has had any
effect.  I assume it's possible to get at more data that way -- please let
me know if you have any idea.


Building
--------

```gcc inverter.c```

There's no Makefile.


Usage
-----

```$ ./a.out```


(More usefully:

```$ nc -l -u -p 10006 | a.out```

as it reads from stdin.)


As shipped, SolaX dataloggers' wifi interfaces start an access point, named
'AP$serialnumber'.  If you connect to that AP, you'll be assigned a 10.* IP
address.  The DL will be at 10.10.100.254 -- login with admin/admin.  Then
change the password.  Be aware that the thing's web interface will happily
accept lengthy passwords, but it discards all but the first 15 characters.
Attempting to login with anything bigger will fail.

You can (and probably should) configure its station mode to connect to your
wifi network: I assume you know enough to do that yourself.

Under the Advanced options you'll see a bit about servers.  Opening that tab
gives a list of three, the top of which will be filled in with 'Default'.
'Default' is the details for solarmanpv.com; you can create an account on
that site with your datalogger's serial number, and it'll draw you graphs.

I prefer to do this sort of thing myself, so under the second entry I've
added my router's address, and told it to use UDP (hence the '-u' above).

Over the coming days I'll add a Perl script to log.  For now, enjoy.

Output is something like this:

```
newdesktop:/home/dickon/src/inverter/ (0) 126 $ ./a.out < /tmp/pkt 
Datalogger serial: 61xxxxx50
Inverter serial: S362xxxxxxx035
Temperature:     29.0 degrees Celsius
PV1 voltage:    146.0 V
PV2 voltage:    183.0 V
PV3 voltage:      0.0 V
PV1 current:      0.0 A
PV2 current:      0.0 A
PV3 current:      0.0 A
P1 voltage:     245.1 V
P2 voltage:       0.0 V
P3 voltage:       0.0 V
Frequency:       50.00 Hz
P1 power:        28 W
P2 power:         0 W
P3 power:         0 W
Today's total:    4.20 kWh
Total:            6.0 kWh

newdesktop:/home/dickon/src/inverter/ (0) 127 $ 
```

Internals
---------

It's a trivial loop.  If it's no good for you, feel free to edit it.

I'm particularly looking out for details of the missing fields, and the
fault condition flags.

The logger spits out two packet (no matter the transport): the first with a
103-byte payload, which is the interesting one, and one 53-byte payload,
which contains the version, serial and MAC numbers of the logger.  Most of
the data seems to be unaligned 16b big-endian pairs.

Looking at a packet from my system, and correlating it with the data on
Solarman, lead me to the following:

```
0x04->0x07             -- Datalogger's serial number backwards.
0x08->0x0b             -- ditto (?)
0x11->0x25             -- Inverter's S/N

0x1f<<8 | 0x20 == 290  -- Temperature, d deg. C.
0x21<<8 | 0x22 == 1357 -- PV1 deciV?
0x23<<8 | 0x24 == 1678 -- PV2 deciV?
0x25<<8 | 0x26 == 0    -- PV3 (unattached) deciV?
0x27<<8 | 0x28 == 0    -- PV1 dA?
0x29<<8 | 0x2a == 0    -- PV2 dA?
0x2b<<8 | 0x2c == 0    -- PV3 dA?
0x2d<<8 | 0x2e == 1    -- AC dA?
0x2f<<8 | 0x30 == 0    -- P2 dA?
0x31<<8 | 0x32 == 0    -- P3 dA?
0x33<<8 | 0x34 == 2451 -- AC deciV?
0x35<<8 | 0x36 == 0    -- ditto, phase 2?
0x37<<8 | 0x38 == 0    -- ditto, phase 3?
0x39<<8 | 0x3a == 5000 -- centiHz?
0x3b<<8 | 0x3c == 28   -- AC W?
0x3d<<8 | 0x3e == 0    -- P2 W?
0x3f<<8 | 0x40 == 0    -- P3 W?
0x45<<8 | 0x46 == 420  -- Today's total, ckWh ?

0x49<<8 | 0x4a == 60   -- Total power output, dWh?
```



Bugs
----

Yeah.  There are.

The problem is, I've had to reverse engineer the packets, and some fields
aren't clear to me.  In particular, the logger is clearly capable of logging
a three-phase inverter, and mine's only a domestic single-phase; I've had to
guess at the meanings of an awful lot of zeroes.  I also haven't made any
headway on the fault conditions that are reportable -- my system currently
trips its AC out some mornings, and these appear on Solarman's site -- but
as I've no documentation to work from, I don't know what to look for.



Licence guff
------------

GPLv2 preamble follows; the licence itself is in COPYING:

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

