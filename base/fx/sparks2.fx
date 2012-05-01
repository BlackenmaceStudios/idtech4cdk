

fx fx/sparks2
{

	{
		delay 0
		duration 0.5
		restart 0
		light "lights/sparks_sound", 1, 1, 1, 96
		offset	0, 0, 0
	}
	{
		delay 0
		sound "electrical_sparks"
		duration 2.5

	}
	{
		delay 0
		duration 2.5
		restart 0
		model "sparky.prt"
		//fadeout 1.5
		//particle "sparks.prt"
	}		   
}
