﻿#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "SpectraFileInfo.h"
#include "ProteinGroup.h"
#include "../Chemistry/Chemistry.h"
using namespace Chemistry;

namespace FlashLFQ
{
    class Identification
    {
    public:
        const std::string BaseSequence;
        const std::string ModifiedSequence;
        const double ms2RetentionTimeInMinutes;
        const double monoisotopicMass;
        SpectraFileInfo *const fileInfo;
        const int precursorChargeState;
        std::unordered_set<ProteinGroup*> proteinGroups;
        ChemicalFormula *const OptionalChemicalFormula;
        const bool UseForProteinQuant;
        double massToLookFor = 0;

        virtual ~Identification()
        {
            //delete fileInfo;
            //delete OptionalChemicalFormula;
        }

        Identification(SpectraFileInfo *fileInfo, const std::string &BaseSequence,
                       const std::string &ModifiedSequence,
                       double monoisotopicMass, double ms2RetentionTimeInMinutes,
                       int chargeState, std::vector<ProteinGroup*> &proteinGroups,
                       ChemicalFormula *optionalChemicalFormula = nullptr,
                       bool useForProteinQuant = true);

        std::string ToString();
        bool Equals( Identification *other );
    };
}
