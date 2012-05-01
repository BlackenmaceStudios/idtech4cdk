// small random sparks
fx fx/smallsparks
{
	{
		restart 1
		random 0.5, 3.5
		duration 5
		rotate 0.1
		particle "particles/smallfiresparks.particle"
		fire blue
	}		   
	{
		delay 1000000
		name blue
		duration 5
		rotate 0.1
		particle "particles/smallbluesparks.particle"
	}		   
}

