------------------------------------------------------------------------------
--    videokiss sample theme file                                                  --
--                                                                          --
--    Copyright (C) 2008 by Alain Basty                                     --
--    alain.basty@free.fr                                                   --
--                                                                          --
--    This program is free software; you can redistribute it and-or modify  --
--    it under the terms of the GNU General Public License as published by  --
--    the Free Software Foundation; either version 2 of the License, or     --
--    (at your option) any later version.                                   --
--                                                                          --
--    This program is distributed in the hope that it will be useful,       --
--    but WITHOUT ANY WARRANTY; without even the implied warranty of        --
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         --
--    GNU General Public License for more details.                          --
--                                                                          --
--    You should have received a copy of the GNU General Public License     --
--    along with this program; if not, write to the                         --
--    Free Software Foundation, Inc.,                                       --
--    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             --
------------------------------------------------------------------------------

-- Movie making rules

clip_rule = {
	"$0",
	"-mix", "25", "-mixer", "luma", "-mixer", "mix:-1"
}

rules = {
	-- Movie files
	{ 
		pattern = "%.mpg$",
		value = clip_rule
	},

	{ 
		pattern = "%.wmv$", 
		value = clip_rule
	},

	{ 
		pattern = "%.mov$", 
		value = clip_rule
	},

	{ 
		pattern = "%.avi$", 
		value = clip_rule
	},

	{ 
		pattern = "%.[jJ][pP][gG]$", 
		value = { "$0", "out=99", "-mix", "25", "-mixer", "luma", "-mixer", "mix:-1" }
	},

	-- Title files
	{
		pattern = "([^/]*) %-%- ([^/]*) %-%- ([^/]*)$", 
		value = {
			"colour:0x33445500", "out=99",
 			"-attach-cut", "watermark:+$2.txt", "producer.align=c", "composite.halign=c", "composite.geometry=0,10%:100%,100%", 
  			"-attach-cut", "watermark:+$3.txt", "producer.align=c", "producer.fgcolour=0xf11fff80", "composite.progressive=1", "composite.valign=b", "composite.halign=c", "composite.geometry=0,0:100%,90%", 
		},
	},
	{
		pattern = "([^/]*) %-%- ([^/]*)$", 
		value = {
-- 			"colour:black", "out=24",
			"colour:black", "out=99",
			"-attach", "watermark:+$2.txt", "composite.progressive=1", "producer.align=centre", "composite.valign=c", "composite.halign=c",
			"-mix", "25", "-mixer", "luma",
			"colour:black", "out=25",
			"-mix", "25", "-mixer", "luma"
		}
	}
}


