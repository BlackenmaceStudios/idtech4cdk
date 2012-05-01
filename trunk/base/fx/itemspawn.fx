fx fx/itemspawn
{
	{
		delay 0
		name "light5"
		duration 0.05
		restart 0
		light "lights/squareblast", 0.6, 0.6, 1, 100
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
		model "itemrespawn.ips"
		offset 0, 0, 16
		fadeOut 2.0
	}		   


}