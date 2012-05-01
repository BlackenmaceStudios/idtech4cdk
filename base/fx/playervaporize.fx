fx fx/playervaporize
{
	{
		delay 0
		name "light5"
		duration 0.005
		restart 0
		light "lights/squareblast", 1, 1, 1, 300
	}
	{										// fade out the blast light
		delay 0.1
		name "light6"
		duration 0.10
		restart 0
		useLight "light5"
		fadeOut 0.005
	}
	{
		delay 0
		duration 1.0
		model "playervaporize.ips"
		offset 0, 0, 0
		fadeOut 1.25
	}		   
}
