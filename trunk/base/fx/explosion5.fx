fx fx/explosion5
{

	{
		delay 0
		duration 0.75
		restart 0
		light "lights/spot01", 1.34, 1.68, 1.94, 350
		offset	0, 0, 16
		noshadows
		//fadeOut 0.25
	}
	{
		delay 0
		duration 0.75
		restart 0
		light "lights/spot01", 1.48, 1.06, 0.5, 350
		offset	0, 0, 16
		noshadows
		//fadeOut 0.25
	}


	{
		delay 0
		duration 2.5
		restart 0
		rotate 0.1
		particle "particles/firesparks.particle"
	}		   

	{
		delay 0
		duration 2.5
		restart 0
		rotate 0.1
		particle "particles/bluesparks.particle"
	}		   


}