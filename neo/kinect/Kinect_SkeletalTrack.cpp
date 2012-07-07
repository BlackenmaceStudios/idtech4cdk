// Kinect_SkeletalTrack.cpp
//

#include "precompiled.h"

#include "kinect_local.h"

const char *kinect_joints[] = {
	"NUI_SKELETON_POSITION_HIP_CENTER",
    "NUI_SKELETON_POSITION_SPINE",
    "NUI_SKELETON_POSITION_SHOULDER_CENTER",
    "NUI_SKELETON_POSITION_HEAD",
    "NUI_SKELETON_POSITION_SHOULDER_LEFT",
    "NUI_SKELETON_POSITION_ELBOW_LEFT",
    "NUI_SKELETON_POSITION_WRIST_LEFT",
    "NUI_SKELETON_POSITION_HAND_LEFT",
    "NUI_SKELETON_POSITION_SHOULDER_RIGHT",
    "NUI_SKELETON_POSITION_ELBOW_RIGHT",
    "NUI_SKELETON_POSITION_WRIST_RIGHT",
    "NUI_SKELETON_POSITION_HAND_RIGHT",
    "NUI_SKELETON_POSITION_HIP_LEFT",
    "NUI_SKELETON_POSITION_KNEE_LEFT",
    "NUI_SKELETON_POSITION_ANKLE_LEFT",
    "NUI_SKELETON_POSITION_FOOT_LEFT",
    "NUI_SKELETON_POSITION_HIP_RIGHT",
    "NUI_SKELETON_POSITION_KNEE_RIGHT",
    "NUI_SKELETON_POSITION_ANKLE_RIGHT",
    "NUI_SKELETON_POSITION_FOOT_RIGHT"
};

#define KJOINT(x) NUI_SKELETON_POSITION_##x

void bmKinectPlayerLocal::UpdatePlayer( NUI_SKELETON_DATA &skeletonData ) {
	memcpy(&this->skeletonData, &skeletonData, sizeof( NUI_SKELETON_DATA ) );

	if(skeletonData.eTrackingState == NUI_SKELETON_TRACKED)
	{
		// We check to see if it failed because otherwise we replace good results with crappy ones.
		if(FAILED(NuiSkeletonCalculateBoneOrientations( &skeletonData, &skeletonOrientation[0] ))) {
			return;
		}

		if(skeletonOrientation[0].endJoint == -842150451)
			return;

		for(int i = 0; i < 20; i++) {
			idVec3 forward, right, up;
			if(skeletonData.eSkeletonPositionTrackingState[i] != NUI_SKELETON_POSITION_TRACKED)
			{
				joints[i] = idQuat(0,0,0,0).ToAngles();
				continue;
			}

	
			if(skeletonOrientation[i].startJoint == KJOINT(HIP_CENTER) &&  skeletonOrientation[i].endJoint == KJOINT(HIP_CENTER))
			{
				joints[i] = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix).ToAngles();
			}
			else if(skeletonOrientation[i].startJoint == KJOINT(SHOULDER_CENTER))
			{
				joints[i] = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(SPINE))
			{
				idMat3 tempMat = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				idMat3 adjustment = idMat3::CreateRotationX( DEG2RAD( 20 ) );
				tempMat *= adjustment;

				idQuat kinectRotation = tempMat.ToQuat();

				joints[i] = idQuat(kinectRotation.y, kinectRotation.z, kinectRotation.x, kinectRotation.w).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(HEAD))
			{
				idMat3 tempMat = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				idMat3 adjustment = idMat3::CreateRotationX( DEG2RAD( -30 ) );
				tempMat *= adjustment;

				idQuat kinectRotation = tempMat.ToQuat();

				joints[i] = idQuat(kinectRotation.y, kinectRotation.z, kinectRotation.x, kinectRotation.w).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(ELBOW_LEFT) || skeletonOrientation[i].endJoint == KJOINT(WRIST_LEFT))
			{
				idMat3 tempMat = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				if(skeletonOrientation[i].endJoint == KJOINT(ELBOW_LEFT)) {
					idMat3 adjustment = idMat3::CreateRotationZ(DEG2RAD(-15));
					tempMat *= adjustment;
				}

				idQuat kinectRotation = tempMat.ToQuat();

				joints[i] = idQuat(-kinectRotation.y, kinectRotation.z, kinectRotation.x, kinectRotation.w).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(HAND_LEFT))
			{
				idMat3 tempMat = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				idMat3 adjustment = idMat3::CreateRotationX( DEG2RAD( -90 ) );
				//tempMat *= adjustment;

				idQuat kinectRotation = tempMat.ToQuat();

				joints[i] = idQuat(kinectRotation.y, kinectRotation.x, -kinectRotation.z, kinectRotation.w).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(ELBOW_RIGHT) || skeletonOrientation[i].endJoint == KJOINT(WRIST_RIGHT))
			{
				idMat3 tempMat = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				if(skeletonOrientation[i].endJoint == KJOINT(ELBOW_RIGHT)) {
					idMat3 adjustment = idMat3::CreateRotationZ(DEG2RAD(15));
					tempMat *= adjustment;
				}

				idQuat kinectRotation = tempMat.ToQuat();

				joints[i] = idQuat(kinectRotation.y, -kinectRotation.z, kinectRotation.x, kinectRotation.w).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(HAND_RIGHT))
			{
				idMat3 tempMat = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				idMat3 adjustment = idMat3::CreateRotationY( DEG2RAD( 90 ) );
				tempMat *= adjustment;

				idQuat kinectRotation = tempMat.ToQuat();

				joints[i] = idQuat(kinectRotation.y,-kinectRotation.x, kinectRotation.z, kinectRotation.w).ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(KNEE_LEFT))
			{
				idMat3 hipLeft = KinectMatrixToEngineMatrix( skeletonOrientation[KJOINT(HIP_LEFT)].hierarchicalRotation.rotationMatrix);
				idMat3 kneeLeft = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				joints[i] = (hipLeft * kneeLeft).ToQuat().ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(KNEE_RIGHT))
			{
				idMat3 hipRight = KinectMatrixToEngineMatrix( skeletonOrientation[KJOINT(HIP_RIGHT)].hierarchicalRotation.rotationMatrix);
				idMat3 kneeRight = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix);

				joints[i] = (hipRight * kneeRight).ToQuat().ToAngles();
			}
			else if(skeletonOrientation[i].endJoint == KJOINT(ANKLE_LEFT) || skeletonOrientation[i].endJoint == KJOINT(ANKLE_RIGHT))
			{
				joints[i] = KinectMatrixToEngineMatrix( skeletonOrientation[i].hierarchicalRotation.rotationMatrix).ToAngles();
			}
			else
			{
				joints[i] = idQuat(0,0,0,0).ToAngles();
			}

			joints[i] = joints[i].ToMat3().ToLeftHandedMatrix().Scale(1, 1, -1).ToAngles(); // Convert this over to a left handed matrix.
	
		}


	}
}

void bmKinectPlayerLocal::GetSkeletonJointPosition( int jointId, idVec3 &pos ) {
	pos.x = skeletonData.SkeletonPositions[jointId].x;
	pos.y = skeletonData.SkeletonPositions[jointId].y;
	pos.z = skeletonData.SkeletonPositions[jointId].z;

}

bool bmKinectPlayerLocal::IsTracking( void ) {
	return skeletonData.eTrackingState == NUI_SKELETON_TRACKED;
}

void bmKinectPlayerLocal::GetJointTransform( int jointId, idQuat &angles ) {
	angles = joints[jointId].ToQuat();
}