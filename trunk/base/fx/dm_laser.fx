fx fx/dm_laser
{
	{
		delay 0
		name "particle2"
		duration 8
		restart 0
		model "mp_d3dm1_laserfire.prt"
	}
	{	// fade out the blast light
		delay 4
		name "part2"
		duration 4
		usemodel "particle2"
		fadeOut 4
	}

}