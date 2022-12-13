#include "fvCFD.h"
#include "turbulenceModel.H"

#include "basicSprayCloud.H"
#include "surfaceFilmModel.H"

#include "SLGThermo.H"
#include "psiChemistryCombustionModel.H"

#include "fixedGradientFvPatchFields.H"
#include "regionProperties.H"
#include "compressibleCourantNo.H"
#include "solidRegionDiffNo.H"
#include "basicSolidThermo.H"


int main(int argc, char *argv[])
{
     #include "setRootCase.H"
     #include "createTime.H"

     regionProperties rp(runTime);

     #include "createFluidMeshes.H"
     #include "createSolidMeshes.H"

     #include "createFluidFields.H"
     #include "createSolidFields.H"

     forAll(fluidRegions, i)
     {

                 Info << "\n___________ Adding to tsurfaceFilm for " << fluidRegions[i].name() << "__________\n" << endl;

                typedef regionModels::surfaceFilmModels::surfaceFilmModel filmModelType;

                tsurfaceFilm.set
                (
                  i,
                  filmModelType::New(fluidRegions[i], gFluid[i])
                );
    }

    #include "initContinuityErrs.H"
    #include "readTimeControls.H"
    #include "readSolidTimeControls.H"
    #include "compressibleMultiRegionCourantNo.H"
    #include "solidRegionDiffusionNo.H"
    #include "setInitialMultiRegionDeltaT.H"

    while(runTime.run())
    {
          Info << "NEW TIMESTEP" << endl;

          #include "readTimeControls.H"
          #include "readSolidTimeControls.H"
          #include "readPimpleControls.H"
          #include "compressibleMultiRegionCourantNo.H"
          #include "solidRegionDiffusionNo.H"
          #include "setMultiRegionDeltaT.H"

        runTime++;

        Info << "Time = " << runTime.timeName() << nl << endl;

        forAll(fluidRegions, i)
        {
                Info << "\n\n----------Solving for spray in fluid region " << fluidRegions[i].name() << "-----------" << endl;

                basicSprayCloud& parcels = parcelsFluid[i];
                parcels.evolve();

                Info << "\n\n-----------Solving for film in fluid region " << fluidRegions[i].name() << "----------" << endl;

                regionModels::surfaceFilmModels::surfaceFilmModel& surfaceFilm = tsurfaceFilm[i];
                surfaceFilm.evolve();
        }

          if(nOuterCorr != 1)
          {
                forAll(fluidRegions, i)
                {
                   #include "setRegionFluidFields.H"
                   #include "storeOldFluidFields.H"
                }
          }

            for(int oCorr=0; oCorr < nOuterCorr; oCorr++)
            {
                  bool finalIter = oCorr = nOuterCorr - 1;

                  forAll(fluidRegions, i)
                  {
                        Info <<  "\n\n-------- Solving for fluid region " << fluidRegions[i].name() << "-----------\n" << endl;

                        #include "setRegionFluidFields.H"
                        #include "readFluidMultiRegionPIMPLEControls.H"
                        #include "solveFluid.H"
                  }

                  forAll(solidRegions, i)
                  {
                     Info << "\n\n-----------Solving for solid region " << solidRegions[i].name() << "----------\n" << endl;

                     #include "setRegionSolidFields.H"
                     #include "readSolidMultiRegionPIMPLEControls.H"
                     #include "solveSolid.H"
                  }
            }


            runTime.write();

            Info << "ExecutionTime = " << runTime.elapsedCpuTime() << " s" << " ClockTime = " << runTime.elapsedClockTime() << " s" << nl << endl;

    }


    Info << "End\n" << endl;



    return 0;




}
