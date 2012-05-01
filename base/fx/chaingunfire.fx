fx fx/chaingunfire
{

	bindto "barrel"
	{
		delay 0
		duration 0.2
		restart 0
		light "muzzleflash", 1, 0.8, 0.4, 250
		offset	0, 0, 16
		//fadeOut 0.25
	}

	{
		delay 0
		duration 3
		restart 0
		particle "particles/chaingunfire.particle"
	}

	{
		delay 0.3
		duration 3
		restart 0
		particle "particles/chaingunsmoke.particle"
	}

}