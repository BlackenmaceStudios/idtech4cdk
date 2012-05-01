fx fx/dm_laser2
{
	{
		delay 0
		name "particle1"
		duration 9
		restart 0
		model "mp_d3dm1_laserembers.prt"
	}
	{	// fade out the blast light
		delay 5
		name "part1"
		duration 4
		usemodel "particle1"
		fadeOut 4
	}

}