fx fx/berserkspawn
{

	{
		delay 0
		name "light5"
		duration 0.05
		restart 0
		light "lights/squareblast", 1, 0, 0, 300
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
		delay 0.1
		duration 2
		model "berserkrespawn.ips"
		offset 0, 0, 16
		fadeOut 1.5
	}		   
}
