kappa = thermo.Cp() * turb.alphaEff();
Kliq = kappa;

const dictionary& patchDict = mesh.lookupObject<dictionary>("surfaceFilmProperties");

word boundaryToWallFilm(patchDict.lookup("patchToWallFilmName"));
word boundaryToSolid(patchDict.lookup("patchToSolidName"));
scalar KLiqValue(readScalar(patchDict.lookup("KValue")));

deltaMap.correctBoundaryConditions();

label bcIndex = mesh.boundaryMesh().findPatchID(boundaryToWallFilm);
label bcSolid = mesh.boundaryMesh().findPatchID(boundaryToSolid);

if(bcIndex != -1)
{
       Info << "Found patch: " << boundaryToWallFilm << endl;
       if(bcSolid = -1)
       {
            FatalError << " patchToSolidName " << boundaryToSolid << " could not be found in dict surfaceFilmProperties!! " << abort(FatalError);
       }

    const scalarField& deltaBC = deltaMap.boundaryField()[bcIndex];
    const labelList& cells = mesh.boundaryMesh()[bcIndex].faceCells();

    forAll(deltaBC, facei)
    {
          if(deltaBC[facei] > 1.0e-6)
          {
              Kliq [cells[facei]] = KLiqValue;
              Kliq.boundaryField() [bcIndex][facei] = KLiqValue;
              Kliq.boundaryField() [bcSolid][facei] = KLiqValue;
          }
    }
}
