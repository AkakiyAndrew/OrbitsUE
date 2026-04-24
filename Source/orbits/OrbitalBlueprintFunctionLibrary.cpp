// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitalBlueprintFunctionLibrary.h"



//struct OrbitParams {
//    double a; // semi-major axis
//    double e; // eccentricity
//};

FVector UOrbitalBlueprintFunctionLibrary::GetOrbitalPosition(double PathFraction, double SemiMajorAxis, double Eccentricity)
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

    return FVector(r * cos(nu), r * sin(nu), 0);
}