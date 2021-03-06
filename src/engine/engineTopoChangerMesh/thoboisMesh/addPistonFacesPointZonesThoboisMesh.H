
    // Add the piston zone
    if (piston().patchID().active() && offSet() > SMALL)
    {

        // Piston position

        label pistonPatchID = piston().patchID().index();

        scalar zPist = max(boundary()[pistonPatchID].patch().localPoints()).z();

        Info << "zPist = " << zPist << endl;

        scalar zPistV = zPist + offSet();

        Info << "zPistV = " << zPistV << endl;

        labelList zone1(faceCentres().size());
        boolList flipZone1(faceCentres().size(), false);
        label nZoneFaces1 = 0;

        bool foundAtLeastOne = false;
        scalar zHigher = GREAT;
        scalar dh = GREAT;
        scalar dl = GREAT;

        forAll (faceCentres(), faceI)
        {
            scalar zc = faceCentres()[faceI].z();
            vector n = faceAreas()[faceI]/mag(faceAreas()[faceI]);
            scalar dd = n & vector(0,0,1);

            if (mag(dd) > 0.1)
            {
                if (zPistV - zc > 0 && zPistV - zc < dl)
                {
                    dl = zPistV - zc;
                }

                if (zc - zPistV > 0 && zc - zPistV < dh)
                {
                    zHigher = zc;
                    dh = zc - zHigher;
                }

                if
                (
                    zc > zPistV - delta()
                    && zc < zPistV + delta()
                )
                {
                    foundAtLeastOne = true;
                    if ((faceAreas()[faceI] & vector(0,0,1)) < 0)
                    {
                        flipZone1[nZoneFaces1] = true;
                    }

                    zone1[nZoneFaces1] = faceI;
                    nZoneFaces1++;
                }
            }
        }

        Info << "Gambit mesh, found " << nZoneFaces1 << " faces for layer addition removal" << endl;
        Info << "Piston patch size = " << boundaryMesh()[piston().patchID().index()].size() << endl;

        // if no cut was found use the layer above
        if (!foundAtLeastOne)
        {

            Info << "NOT FOUND AT LEAST ONE" << endl;
            Info << "zHigher = " << zHigher << endl;

            zPistV = zHigher;

            forAll (faceCentres(), faceI)
            {
                scalar zc = faceCentres()[faceI].z();
                vector n = faceAreas()[faceI]/mag(faceAreas()[faceI]);
                scalar dd = n & vector(0,0,1);
                if (mag(dd) > 0.1)
                {

                    if
                    (
                        zc > zPistV - delta()
                        && zc < zPistV + delta()
                    )
                    {
                        if ((faceAreas()[faceI] & vector(0,0,1)) < 0)
                        {
                            flipZone1[nZoneFaces1] = true;
                        }

                        zone1[nZoneFaces1] = faceI;
                        nZoneFaces1++;
                    }
                }
            }

        }

        zone1.setSize(nZoneFaces1);
        flipZone1.setSize(nZoneFaces1);

//        fz[nFaceZones]=
        fz.append
        (
            new faceZone
            (
                "pistonLayerFaces",
                zone1,
                flipZone1,
                nFaceZones,
                faceZones()
            )
        );

        nFaceZones++;


        // Construct point zones


        // Points below the piston which moves with the piston displacement
        DynamicList<label> pistonPoints(nPoints() / 10);

        forAll (points(), pointI)
        {
            scalar zCoord = points()[pointI].z();

            if (zCoord > deckHeight() - delta())
            {
            }
            else if (zCoord < zPistV + delta())
            {
                pistonPoints.append(pointI);
                //Info<< "PistonPoint:" << pointI << " coord:" << points[pointI]
                //    << endl;
            }
        }


        pz.append
        (
            new pointZone
            (
                "pistonPoints",
                pistonPoints.shrink(),
                nPointZones,
                pointZones()
            )
        );

        nPointZones++;

    }
    else if(piston().patchID().active() && offSet() <= SMALL)
    {
        label pistonPatchID = piston().patchID().index();

        const polyPatch& pistonPatch =
            boundaryMesh()[piston().patchID().index()];

        labelList pistonPatchLabels(pistonPatch.size(), pistonPatch.start());

        forAll (pistonPatchLabels, i)
        {
            pistonPatchLabels[i] += i;
        }

//        fz[nFaceZones] =
        fz.append
        (
            new faceZone
            (
                "pistonLayerFaces",
                pistonPatchLabels,
                boolList(pistonPatchLabels.size(), true),
                nFaceZones,
                faceZones()
            )
        );
        nFaceZones++;
        // Construct point zones

        scalar zPistV = max(boundary()[pistonPatchID].patch().localPoints()).z();

        // Points below the piston which moves with the piston displacement
        DynamicList<label> pistonPoints(nPoints() / 10);

        forAll (points(), pointI)
        {
            scalar zCoord = points()[pointI].z();

            if (zCoord > deckHeight() - delta())
            {
            }
            else if (zCoord < zPistV + delta())
            {
                pistonPoints.append(pointI);
                //Info<< "PistonPoint:" << pointI << " coord:" << points[pointI]
                //    << endl;
            }
        }

        pz.append
        (
            new pointZone
            (
                "pistonPoints",
                pistonPoints.shrink(),
                nPointZones,
                pointZones()
            )
        );

        nPointZones++;

    }
