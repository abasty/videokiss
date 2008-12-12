
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
		pattern = "%.avi$", 
		value = clip_rule
	},

	-- Title files
	{
		pattern = "([^/]*) %-%- ([^/]*) %-%- ([^/]*)$", 
		value = {
			"colour:black", "out=74",
 			"-attach-cut", "watermark:+$2.txt", "composite.progressive=1", "composite.valign=c", "composite.halign=c", 
  			"-attach-cut", "watermark:+$3.txt", "producer.fgcolour=red", "composite.progressive=1", "composite.valign=bottom", "composite.halign=r", 
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


