
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
		pattern = "(.*) %-%- (.*)", 
		value = {
			"colour:0xFFFF00", "out=24",
			-- producer : defines the current producer caracteristics (here the watermark). "producer" must be present in
			-- an -attach command. See the "watermark" producer doc. to know about the right properties. The out property does not need producer
			-- because it is an initialization property (constructor?). 
			-- geometry = X,Y:WxH[!][:mix] (mix = alpha)
			"-attach-cut", "watermark:+<i>toto</i>.txt", "producer.fgcolour=green", "producer.font=Sans 128", "producer.weight=800", "out=12", "composite.geometry=50%,50%:100x300!:50",
			-- composite : defines the current composer caracteristics (here the watermark) like the position
			"-attach-cut", "watermark:+titi.txt", "composite.valign=bottom", 
			"colour:blue", "out=2",
		},
		valueOld = {
			"colour:red", "out=24",
			"colour:red", "out=99",
			"-attach", "watermark:+$2.txt", "fgcolour=0x000000ff", "composite.progressive=0", "composite.valign=bottom", "composite.halign=c", 
			"-mix", "25", "-mixer", "luma",
			"colour:red", "out=25",
			"-mix", "25", "-mixer", "luma"
		}
	},
}


