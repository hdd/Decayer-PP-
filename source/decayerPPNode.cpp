
#include <stdlib.h>

//
// 
// File: decayerPPNode.cpp
//
// Dependency Graph Node: decayerPP
//
// Author: Maya Plug-in Wizard 2.0
//

#include "decayerPPNode.h"
#include <maya/MDoubleArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MVector.h>
#include <maya/MGlobal.h>
#include <math.h>
#include <ANN/ANN.h>
#include <math.h>
#include <maya/MTime.h>
#include <maya/MFnCompoundAttribute.h>

#define SPACE 3

#define McheckErr(stat,msg)                                         \
	if ( MS::kSuccess != stat ) {                               \
		MGlobal::displayError(MString("STAT ERROR :")+msg); \
		return MS::kFailure;                                \
	}


MTypeId         decayerPP::id( 0x97012 );

//      node attributes
// 
MObject		decayerPP::scaleFactor;		
MObject		decayerPP::inputPostionPP;		
MObject		decayerPP::outputDistancePP;

MObject		decayerPP::inputLocatorPos;
MObject		decayerPP::inputLocatorPosX;
MObject		decayerPP::inputLocatorPosY;
MObject		decayerPP::inputLocatorPosZ;

//      static ANN variables
//
ANNkd_tree*     decayerPP::kdTree; 
ANNidxArray     decayerPP::ids;
ANNdistArray    decayerPP::dists;
ANNpointArray   decayerPP::pa; 
ANNpoint        decayerPP::P;

// functions
double clamp(double val,double min , double max);
double lerp (double start, double end, double p);
void assignANNPts(ANNpointArray& pa,MFnVectorArrayData mv);
        
decayerPP::decayerPP() {
    system("clear");
}

decayerPP::~decayerPP() { 
    
    /*
    cout  << "start closing ANN..." << endl;
    cout << "dealloc pa" << endl;
    annDeallocPts(decayerPP::pa);
    cout << "dealloc P" << endl;
    annDeallocPt(decayerPP::P);  
    cout << "delete  ids" << endl;
    delete [] decayerPP::ids;
    cout << "delete dists" << endl;
    delete [] decayerPP::dists;
    cout << "delete kdtree" << endl;
    delete decayerPP::kdTree;
    cout  << "close ANN..." << endl;
    annClose();
    cout  << "ANN...closed" << endl;
     */ 
}

MStatus decayerPP::compute( const MPlug& plug, MDataBlock& data )

{
	MStatus returnStatus;
 
	if( plug == outputDistancePP )
	{
		
                MDataHandle inputLocPosH = data.inputValue( inputLocatorPos, &returnStatus );
                McheckErr(returnStatus,"inputLocPosH");
                
		MDataHandle inputPostionPPH = data.inputValue( inputPostionPP, &returnStatus );
                McheckErr(returnStatus,"inputPostionPPH");
                
		MDataHandle scaleFactorH = data.inputValue( scaleFactor, &returnStatus );
                McheckErr(returnStatus,"scaleFactorH");
                
                MDataHandle timeH = data.inputValue(time,&returnStatus);
                McheckErr(returnStatus,"timeH");
               
		MDataHandle outMinValH = data.inputValue( outMinValue, &returnStatus );
                McheckErr(returnStatus,"outMinValH");
                
		MDataHandle outMaxValH = data.inputValue( outMaxValue, &returnStatus );
                McheckErr(returnStatus,"outMaxValH");
                
		MDataHandle outputDistancePPH = data.outputValue( outputDistancePP,&returnStatus );
                McheckErr(returnStatus,"outputDistancePPH");
                MTime time = timeH.asTime();
                
                //cout << "parsing particles pos" << endl;
		MFnVectorArrayData inputPostionPPFn =MFnVectorArrayData( inputPostionPPH.data(),&returnStatus);
                McheckErr(returnStatus,"inputPostionPPFn");
                
                
                MTime ctime = timeH.asTime();
                //cout << "time "<< ctime << endl;
                //MDataHandle inputLocPosH = data.inputValue( inputLocatorPos, &returnStatus );
                MVector vLocPos= MVector(inputLocPosH.asDouble3());
                
                //cout << "parsing outMin & outMax" << endl;
		double minv=outMinValH.asDouble();
		double maxv=outMaxValH.asDouble();
                
//                cout << "parsing inPosPP" << endl;
		int numP=inputPostionPPFn.length();
                
//                cout << "parsing scale" << endl;    
                double scale = scaleFactorH.asDouble();
                
		MDoubleArray outputDistancePP = MDoubleArray(numP);
		//MVector vLocPos = inputLocPosH.asVector();
		
//                cout << "xPosLoc "<< vLocPos.x << endl;
                
                //cout << "time: " << ttime << endl;
                if (numP != 0){                    

//                    cout << "deallocating decayerPP::pa"<< endl;                                     
                    if (decayerPP::pa != NULL) annDeallocPts(decayerPP::pa);   
//                        cout << "\talloc ANN Pts\t" << numP << "\tparticles" << endl;
                        decayerPP::pa = annAllocPts(numP*SPACE,SPACE);                   
                    
                        
//                    cout << "deallocating decayerPP::P"<< endl;
                    if (decayerPP::P != NULL) annDeallocPt(decayerPP::P);      
//                        cout << "\tallocating annAllocPt decayerPP::P" << endl;
                        decayerPP::P = annAllocPt(SPACE);
                        P[0]=vLocPos.x;
                        P[1]=vLocPos.y;
                        P[2]=vLocPos.z;   
                        
//                    cout <<"P:"<< P[0]<<":"<< P[1]<< ":"<<P[2] << endl;
                        
//                    cout << "deleting decayerPP::ids"<< endl;
                    if ( decayerPP::ids != NULL) delete [] decayerPP::ids ;
//                        cout << "\tallocating memory for decayerPP::ids " << endl;
                        decayerPP::ids = new ANNidx[numP];
                    
//                    cout << "deleting decayerPP::dists"<< endl;
                    if ( decayerPP::dists != NULL) delete [] decayerPP::dists;  
//                        cout << "\tallocating memory for decayerPP::dists" << endl;
                        decayerPP::dists = new ANNdist[numP];
                   
//                     cout << "deallocating decayerPP::kdTree"<< endl;
                    if (decayerPP::kdTree != NULL) delete decayerPP::kdTree;
                    //if (decayerPP::kdTree != NULL) decayerPP::kdTree->~ANNkd_tree();
//                        cout << "\tassign ANN Pts\t"<< endl;
                        assignANNPts(pa,inputPostionPPFn);  
//                        cout << "\tcreateTree \t"<< endl;
                        decayerPP::kdTree= new ANNkd_tree(decayerPP::pa,numP,SPACE);               
                    
//                    cout << "\tstart search" << endl;
                      //decayerPP::kdTree->annkSearch(decayerPP::P,numP,decayerPP::ids,decayerPP::dists,0.0);
                    
//                    cout << " dist [0] "<< decayerPP::dists[0]<< endl;
//                    cout << " dist [ids[0]] "<< decayerPP::dists[decayerPP::ids[0]]<< endl;
                        cout  << "ann" << decayerPP::pa[0][0] << ":" << decayerPP::pa[0][1] << ":" << decayerPP::pa[0][2] << endl;
                        cout  << "part" << inputPostionPPFn[0].x << " : "<< inputPostionPPFn[0].y << " : "<< inputPostionPPFn[0].z << endl;
                        cout << "-------------------------------------------------------" << endl;                                
                    for(int j =0 ;j<numP;j++){
                        double distPP; 
                        double linDist;
                        ANNdist pdist =annDist(SPACE,P,decayerPP::pa[j]);
                        linDist=lerp(0,scale,pdist);
                        //outputDistancePP[j]=clamp(linDist,minv,maxv); 
                        //linDist=lerp(0,scale,);
                        outputDistancePP[j]=clamp(linDist,minv,maxv);
                    }
//                    cout << "\tsetting the new outDistance done..." << endl;
//                    cout << "-------------------------------------------------------" << endl;
                }
                
		MObject objResult =  MFnDoubleArrayData().create(outputDistancePP);
		outputDistancePPH.setMObject(objResult);
		data.setClean(plug);
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* decayerPP::creator()
{
	return new decayerPP();
}

MStatus decayerPP::initialize()
	
{
        MStatus returnStatus;
	
        MFnNumericAttribute	nAttr;
	MFnTypedAttribute	tAttr;
        MFnCompoundAttribute	cAttr;
        
        
	scaleFactor = nAttr.create( "scale", "s", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
 	nAttr.setStorable(true);
	nAttr.setWritable( true );
	nAttr.setReadable( false );

	// "inPositions"
	inputPostionPP = tAttr.create( "inPositionPP", "ipp", MFnData::kVectorArray );
	tAttr.setReadable( false );
	tAttr.setWritable( true );
	tAttr.setStorable( true );
	
        
        inputLocatorPosX=nAttr.create("inputLocPosX", "inLX",MFnNumericData::kDouble);
 	nAttr.setKeyable(true);
 	nAttr.setStorable(true);
	nAttr.setWritable( true );
	nAttr.setReadable( false );
        
        inputLocatorPosY=nAttr.create("inputLocPosY", "inLY",MFnNumericData::kDouble);
 	nAttr.setKeyable(true);
 	nAttr.setStorable(true);
	nAttr.setWritable( true );
	nAttr.setReadable( false );     
        
        inputLocatorPosZ=nAttr.create("inputLocPosZ", "inLZ",MFnNumericData::kDouble);
 	nAttr.setKeyable(true);
 	nAttr.setStorable(true);
	nAttr.setWritable( true );
	nAttr.setReadable( false ); 
        
	inputLocatorPos = cAttr.create( "inputLocPos", "inL");
        cAttr.addChild(inputLocatorPosX);
        cAttr.addChild(inputLocatorPosY);
        cAttr.addChild(inputLocatorPosZ);

        // OUT PARTICLES ATTR
	MDoubleArray defaultDoubleArray;
	MFnDoubleArrayData doubleArrayDataFn;
	doubleArrayDataFn.create(defaultDoubleArray);
	tAttr.create( "outDistancePP", "od", MFnData::kDoubleArray,doubleArrayDataFn.object());
	outputDistancePP = tAttr.object();
	tAttr.setReadable( true );
	tAttr.setWritable( false );
	tAttr.setStorable( false );


	// Add the attributes we have created to the node
	//
	addAttribute( time );
	addAttribute( outMinValue );
	addAttribute( outMaxValue );
	addAttribute( scaleFactor );
	addAttribute( inputPostionPP );
	addAttribute( inputLocatorPos );
	addAttribute( outputDistancePP );

	//addAttribute( outValuePP  );

	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	//
	attributeAffects( time, outputDistancePP );
	attributeAffects( outMinValue, outputDistancePP );
	attributeAffects( outMaxValue, outputDistancePP );
	attributeAffects( scaleFactor, outputDistancePP );
	attributeAffects( inputPostionPP, outputDistancePP );
        
	attributeAffects( inputLocatorPos, outputDistancePP );
        attributeAffects( inputLocatorPosX, outputDistancePP );
        attributeAffects( inputLocatorPosY, outputDistancePP );
        attributeAffects( inputLocatorPosZ, outputDistancePP );

	return MS::kSuccess;

}


double lerp (double start, double end, double p){
	double diff = end - start;
	return p/(diff-start);
}


double clamp(double val,double min , double max){

	if (val < min){ 
		return min;
	}
	if (val > max)
	{
		return max;
	}
	return val;
}

void assignANNPts(ANNpointArray& pu,MFnVectorArrayData mv){
    for (int i=0;i <mv.length();i++){
        for (int d=0;d< SPACE;d+=SPACE){
            pu[i][d] =mv[i][d];
            pu[i][d+1] =mv[i][d+1];
            pu[i][d+2] =mv[i][d+2];   
       }
    }
}


