fx fx/smallfire
{

	{
		delay 0
		duration 10
		restart 0
		light "lights/roundfire", 0.74, 0.58, 0.25, 250
		offset	0, 0, 16
		//fadeOut 0.25
	}


	{
		delay 0.2
		duration 10
		restart 0
		rotate 0.1
		particle "particles/smallflame.particle"
	}		   

	{
		delay 0
		duration 10
		restart 0
		rotate 0.1
		particle "particles/smallflamesmoke.particle"
	}		   

	{
		delay 0.3
		duration 10
		restart 0
		rotate 0.1
		particle "particles/smallfireembers.particle"
	}		   


}