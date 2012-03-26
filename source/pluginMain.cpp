//
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
#include <ANN/ANN.h>
#include "decayerPPNode.h"
#include <maya/MFnPlugin.h>


MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "hdd", "8.5", "Any");

	status = plugin.registerNode( "decayerPP", decayerPP::id, decayerPP::creator,
		decayerPP::initialize ,MPxNode::kParticleAttributeMapperNode);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( decayerPP::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
