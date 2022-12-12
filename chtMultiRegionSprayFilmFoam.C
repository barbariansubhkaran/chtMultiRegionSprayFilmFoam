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
                
        }

    }







}
