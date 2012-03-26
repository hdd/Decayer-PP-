#ifndef _decayerPPNode
#define _decayerPPNode
//
// 
// File: decayerPPNode.h
//
// Dependency Graph Node: decayerPP
//
// Author: Maya Plug-in Wizard 2.0
//
#include <maya/MPxParticleAttributeMapperNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MFnVectorArrayData.h>
#include <ANN/ANN.h>
 
class decayerPP : public MPxParticleAttributeMapperNode
{
public:
					decayerPP();
	virtual				~decayerPP(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:

        //  LOCATOR
	static	MObject		scaleFactor ;
	static  MObject		inputLocatorPos ,inputLocatorPosX,inputLocatorPosY,inputLocatorPosZ;
        
        // PARTICELLE IN
	static  MObject		inputPostionPP;		
	
        // PARTICELLE OUT
	static  MObject		outputDistancePP;		

	// NODE ID
	static	MTypeId	id;
        
        // ANN
	static ANNkd_tree* kdTree;
        static ANNidxArray ids;
        static ANNdistArray  dists;
        static ANNpointArray pa;
        static ANNpoint P;
        

};

#endif
