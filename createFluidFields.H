PtrList<SLGThermo> thermoFluid(fluidRegions.size());
PtrList<volScalarField> rhoFluid(fluidRegions.size());
PtrList<volScalarField> kappaFluid(fluidRegions.size());
PtrList<volVectorField> UFluid(fluidRegions.size());
PtrList<surfaceScalarField> phiFluid(fluidRegions.size());
PtrList<uniformDimensionedVectorField> gFluid(fluidRegions.size());
PtrList<compressible::turbulenceModel> turbulence(fluidRegions.size());
PtrList<volScalarField> p_rghFluid(fluidRegions.size());
PtrList<volScalarField> ghFluid(fluidRegions.size());
PtrList<surfaceScalarField> ghfFluid(fluidRegions.size());
PtrList<volScalarField> KFluid(fluidRegions.size());
PtrList<volScalarField> dpdtFluid(fluidRegions.size());


PtrList<combustionModels::psiChemistryCombustionModel> combustion(fluidRegions.size());
PtrList<volScalarField> dQFluid(fluidRegions.size());
PtrList<multivariateSurfaceInterpolationScheme<scalar>::fieldTable> fields(fluidRegions.size());
PtrList<basicSprayCloud> parcelsFluid(fluidRegions.size());
PtrList<volScalarField> KliqFluid(fluidRegions.size());
PtrList<volScalarField> deltaMapFluid(fluidRegions.size());
PtrList<regionModels::surfaceFilmModels::surfaceFilmModel> tsurface(fluidRegions.size());


List<scalar> initialMassFluid(fluidRegions.size());

forAll(fluidRegions, i)
{
      Info << "\n-------- Creating fluidRegion" << fluidRegion[i].name() << "---------\n" << endl;

      combustion.set
      (
         i,
         combustionModels::psiChemistryCombustionModel::New
         (
           fluidRegions[i]
         )

      );

       psiChemistryModel& chemistry = combustion[i].pChemistry();

       hsCombustionThermo& thermo = chemistry.thermo();

       volScalarField& hs = thermo.hs();

       basicMultiComponentMixture& composition = thermo.composition();
       PtrList<volScalarField>& Y = composition.Y();

       Info << "*** Reading fluid mesh thermophysical properties for region " << fluidRegions[i].name() << nl << endl;

       Info << "    Adding to thermoFluid\n" << endl;

       thermoFluid.set
       (
              i,
              new SLGThermo(fluidRegions[i], thermo)
       );

       Info << "   Adding to rhoFluid\n" << endl;

       rhoFluid.set
       (
          i,
          new volScalarField
          (
                IOobject
                (
                    "rho",
                    runTime.timeName(),
                    fluidRegions[i],
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),

                thermo.rho()
          )
       );

       Info << "    Adding to kappaFluid\n" << endl;

       kappaFluid.set
       (
              i,
              new volScalarField
              (
                   IOobject
                   (
                      "kappa",
                      runTime.timeName(),
                      fluidRegions[i],
                      IOobject::NO_READ,
                      IOobject::NO_WRITE
                   ),

                   thermo.Cp() * thermo.alpha()
              )
       );

    Info << "     Adding to UFluid\n" << endl;

    UFluid.set
    (
          i,
          new volVectorField
          (
                IOobject
                (
                     "U",
                     runTime.timeName(),
                     fluidRegions[i],
                     IOobject::MUST_READ,
                     IOobject::AUTO_WRITE
                ),

                fluidRegions[i]
          )
    );

 Info << "    Adding to phiFluid\n" << endl;

    phiFluid.set
    (
        i,
        new surfaceScalarField
        (
             IOobject
             (
                  "phi"
                  runTime.timeName(),
                  fluidRegions[i],
                  IOobject::READ_IF_PRESENT,
                  IOobject::AUTO_WRITE
             ),

             linearInterpolate(rhoFluid[i] * UFluid[i])
                & fluidRegions[i].Sf()
        )
    );

     Info " Adding to gFluid\n" << endl;

     gFluid.set
     (
            i,
            new uniformDimensionedVectorField
            (
                 IOobject
                 (
                      "g"
                      runTime.constant(),
                      fluidRegions[i],
                      IOobject::MUST_READ,
                      IOobject::NO_WRITE
                 )
            )
     );

     Info << "      Adding to turbulence\n" << endl;

     turbulence.set
     (
        i,
        compressible::turbulenceModel::New
        (
              rhoFluid[i]
              UFluid[i]
              phiFluid[i]
              thermo
        )


     );


     Info << "       Adding to ghFluid\n" << endl;

     ghFluid.set
     (
        i,
        new volScalarField("gh", gFluid[i] & fluidRegions[i].C())



     );

     Info << "    Adding to ghfFluid\n" << endl;

     ghfFluid.set
     (
          i,
          new surfaceScalarField("ghf", gFluid[i] & fluidRegions[i].Cf())
     );

    p_rghFluid.set
    (
        i,
        new volScalarField
        (
             IOobject
             (
                  "p_rgh",
                  runTime.timeName(),
                  fluidRegions[i],
                  IOobject::MUST_READ,
                  IOobject::AUTO_WRITE
             ),

             fluidRegions[i]
        )
    );

    p_rghFluid[i] = thermo.p() - rhoFluid[i] * ghFluid[i];

    initialMassFluid[i] = fvc::domainIntegrate(rhoFluid[i]).value();

    Info << "      Adding to KFluid\n" << endl;

    KFluid.set
    (
            i,
            new volScalarField
            (
               "K"
                0.5 * magSqr(UFluid[i])
            )
    );

    Info << "     Adding to dpdtFluid\n" << endl;

    dpdtFluid.set
    (
       i,
       new volScalarField
       (
            "dpdt",
            fvc::ddt(thermo.p())
       )


    );

    Info << "    Adding to dQFluid\n" << endl;

    dQFluid.set
    (
         i,
        new volScalarField
        (
              IOobject
              (
                 "dQ",
                 runTime.timeName(),
                 fluidRegions[i],
                 IOobject::NO_READ,
                 IOobject::AUTO_WRITE
              ),

              fluidRegions[i],
              dimensionedScalar("dQ", dimEnergy/dimTime, 0.0)
        )
    );

   multivariateSurfaceInterpolationScheme<scalar>::fieldTable * fieldsi = new multivariateSurfaceInterpolationScheme<scalar>::fieldTable;

   forAll(Y, j)
   {
         fieldsi->add(Y[j]);
   }

    fieldsi->add(hs);
    fields.set(i, fieldsi);

    Info << "    Adding to parcelsFluid\n" << endl;

    parcelsFluid.set
    (
        i,
        new basicSprayCloud
        (
          "sprayCloud",
          rhoFluid[i],
          UFluid[i],
          gFluid[i],
          thermoFluid[i]
        )

    );

      Info << "     Adding to KliqFluid\n" << endl;
      KliqFluid.set
      (
              i,
              new volScalarField
              (
                    IOobject
                    (
                         "Kliq"
                         runTime.timeName(),
                         fluidRegions[i],
                         IOobject::NO_READ,
                         IOobject::AUTO_WRITE
                    ),

                    thermo.Cp() * thermo.alpha()
              )
      );

      Info << "       Adding to deltaMapFluid\n" << endl;
      deltaMapFluid.set
      (
           i,
           new volScalarField
           (
                  IOobject
                  (
                        "deltaMap",
                        runTime.timeName(),
                        fluidRegions[i],
                        IOobject::MUST_READ,
                        IOobject::AUTO_WRITE
                  ),

                  fluidRegions[i]
           )
      );
}
