// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitalBlueprintFunctionLibrary.h"



//struct OrbitParams {
//    double a; // semi-major axis
//    double e; // eccentricity
//};

FVector UOrbitalBlueprintFunctionLibrary::GetOrbitalPosition(
    double PathFraction, 
    double SemiMajorAxis, 
    double Eccentricity,
    double Inclination,               // inclination (degrees)
    double AscendingNodeLongitude,    // longitude of ascending node (degrees)
    double PeriapsisArgument          // argument of periapsis (degrees)
    )
{
    double M = 2.0 * PI * PathFraction;

    // Solve Kepler: M = E - e sin E
    double E = M;
    for (int i = 0; i < 8; ++i)
    {
        double f = E - Eccentricity * sin(E) - M;
        double fp = 1.0 - Eccentricity * cos(E);
        E -= f / fp;
    }

    double cosE = cos(E);
    double sinE = sin(E);

    double r = SemiMajorAxis * (1.0 - Eccentricity * cosE);

    double nu = 2.0 * atan2(
        sqrt(1.0 + Eccentricity) * sinE,
        sqrt(1.0 - Eccentricity) * (1.0 + cosE)
    );

    
    double cosO = cos(FMath::DegreesToRadians(AscendingNodeLongitude));
    double sinO = sin(FMath::DegreesToRadians(AscendingNodeLongitude));
    double cosi = cos(FMath::DegreesToRadians(Inclination));
    double sini = sin(FMath::DegreesToRadians(Inclination));

    double cos_wv = cos(FMath::DegreesToRadians(PeriapsisArgument) + nu);
    double sin_wv = sin(FMath::DegreesToRadians(PeriapsisArgument) + nu);

    double x = r * (cosO * cos_wv - sinO * sin_wv * cosi);
    double y = r * (sinO * cos_wv + cosO * sin_wv * cosi);
    double z = r * (sin_wv * sini);


    return FVector(x, y, z);
}