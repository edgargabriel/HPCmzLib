﻿#include "TestProteinDigestion.h"
#include "../Proteomics/Protein/Protein.h"
#include "../Proteomics/Modifications/Modification.h"
#include "../MassSpectrometry/Enums/DissociationType.h"
#include "../Proteomics/Protein/ProteolysisProduct.h"
#include "../Proteomics/Modifications/ModificationMotif.h"
#include "../Chemistry/ChemicalFormula.h"
#include "../Chemistry/PeriodicTable.h"

using namespace Chemistry;
using namespace MassSpectrometry;
using namespace NUnit::Framework;
using namespace Proteomics;
using namespace Proteomics::AminoAcidPolymer;
using namespace Proteomics::Fragmentation;
using namespace Proteomics::ProteolyticDigestion;
namespace Stopwatch = System::Diagnostics::Stopwatch;

namespace Test
{

System::Diagnostics::Stopwatch *TestProteinDigestion::privateStopwatch;

    Stopwatch *TestProteinDigestion::getStopwatch()
    {
        return privateStopwatch;
    }

    void TestProteinDigestion::setStopwatch(Stopwatch *value)
    {
        privateStopwatch = value;
    }

    void TestProteinDigestion::Setup()
    {
        Stopwatch tempVar();
        setStopwatch(&tempVar);
        getStopwatch()->Start();
    }

    void TestProteinDigestion::TearDown()
    {
        std::cout << StringHelper::formatSimple("Analysis time: {0}h {1}m {2}s", getStopwatch()->Elapsed.Hours, getStopwatch()->Elapsed.Minutes, getStopwatch()->Elapsed.Seconds) << std::endl;
    }

    void TestProteinDigestion::TestGoodPeptide()
    {
        auto prot = new Protein("MNNNKQQQQ", "");
        auto motifList = DigestionMotif::ParseDigestionMotifsFromString("K|");
        auto protease = new Protease("CustomizedProtease", CleavageSpecificity::Full, "", "", motifList);
        ProteaseDictionary::getDictionary().emplace(protease->getName(), protease);
        DigestionParams *digestionParams = new DigestionParams(protease->getName(), 0, 1, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        std::vector<Modification*> variableModifications;
        auto ye = prot->Digest(digestionParams, std::vector<Modification*>(), variableModifications).ToList();

        Assert::AreEqual(2, ye.size());

        auto pep1 = ye[0];
        Assert::IsTrue(pep1.MonoisotopicMass > 0);

        auto test = pep1.Fragment(DissociationType::HCD, FragmentationTerminus::Both);

        for (auto huh : pep1.Fragment(DissociationType::HCD, FragmentationTerminus::Both))
        {
            Assert::IsTrue(huh->NeutralMass > 0);
        }
        auto pep2 = ye[1];
        Assert::IsTrue(pep2.MonoisotopicMass > 0);
        for (auto huh : pep2.Fragment(DissociationType::HCD, FragmentationTerminus::Both))
        {
            Assert::IsTrue(huh->NeutralMass > 0);
        }

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
//C# TO C++ CONVERTER TODO TASK: A 'delete protease' statement was not added since protease was passed to a method or constructor. Handle memory management manually.
        delete prot;
    }

    void TestProteinDigestion::TestNoCleavage()
    {
        std::vector<Modification*> fixedModifications;
        auto prot = new Protein("MNNNKQQQQ", nullptr, nullptr, nullptr, std::unordered_map<int, std::vector<Modification*>>(), std::vector<ProteolysisProduct*> {new ProteolysisProduct(std::make_optional(5), std::make_optional(6), "lala")});
        DigestionParams *digestionParams = new DigestionParams("trypsin", 2, 5, int::MaxValue, 1024, InitiatorMethionineBehavior::Variable, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        auto ye = prot->Digest(digestionParams, fixedModifications, std::vector<Modification*>()).ToList();
        Assert::AreEqual(3, ye.size());

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
        delete prot;
    }

    void TestProteinDigestion::TestBadPeptide()
    {
        auto prot = new Protein("MNNNKQQXQ", "");
        auto motifList = DigestionMotif::ParseDigestionMotifsFromString("K|");
        auto protease = new Protease("Custom Protease7", CleavageSpecificity::Full, "", "", motifList);
        ProteaseDictionary::getDictionary().emplace(protease->getName(), protease);
        DigestionParams *digestionParams = new DigestionParams(protease->getName(), 0, 1, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        auto ye = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();

        Assert::AreEqual(2, ye.size());
        auto pep1 = ye[0];
        Assert::IsTrue(pep1.MonoisotopicMass > 0);
        for (auto huh : pep1.Fragment(DissociationType::HCD, FragmentationTerminus::Both))
        {
            Assert::IsTrue(huh->NeutralMass > 0);
        }

        auto pep2 = ye[1];
        Assert::IsNaN(pep2.MonoisotopicMass);
        auto cool = pep2.Fragment(DissociationType::HCD, FragmentationTerminus::Both)->ToArray();
        Assert::IsTrue(cool[0].NeutralMass > 0);
        Assert::IsTrue(cool[1].NeutralMass > 0);
        Assert::IsTrue(cool[3].NeutralMass > 0);
        Assert::IsTrue(std::isnan(cool[2].NeutralMass));
        Assert::IsTrue(std::isnan(cool[4].NeutralMass));
        Assert::IsTrue(std::isnan(cool[5].NeutralMass));
        Assert::IsTrue(cool.size() == 6);

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
//C# TO C++ CONVERTER TODO TASK: A 'delete protease' statement was not added since protease was passed to a method or constructor. Handle memory management manually.
        delete prot;
    }

    void TestProteinDigestion::TestPeptideWithSetModifications()
    {
        auto prot = new Protein("M", "");
        DigestionParams *digestionParams = new DigestionParams("trypsin", 0, 1, int::MaxValue, 1024, InitiatorMethionineBehavior::Variable, 3, CleavageSpecificity::Full, FragmentationTerminus::Both); // if you pass Custom Protease7 this test gets really flakey.
        std::vector<Modification*> variableModifications;
        ModificationMotif motif;
        ModificationMotif::TryGetMotif("M", motif);

        Modification tempVar("ProtNmod", "", "", "", motif, "N-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        variableModifications.push_back(&tempVar);
        Modification tempVar2("pepNmod", "", "", "", motif, "Peptide N-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        variableModifications.push_back(&tempVar2);
        Modification tempVar3("resMod", "", "", "", motif, "Anywhere.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        variableModifications.push_back(&tempVar3);
        Modification tempVar4("PepCmod", "", "", "", motif, "Peptide C-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        variableModifications.push_back(&tempVar4);
        Modification tempVar5("ProtCmod", "", "", "", motif, "C-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        variableModifications.push_back(&tempVar5);

        auto ye = prot->Digest(digestionParams, std::vector<Modification*>(), variableModifications).ToList();
        Assert::AreEqual(3 * 2 * 3, ye.size());
        Assert::AreEqual("[H]M[H][H]", ye.back().SequenceWithChemicalFormulas);

        double m1 = 5 * PeriodicTable::GetElement("H")->getPrincipalIsotope()->getAtomicMass() + Residue::ResidueMonoisotopicMass['M'] + PeriodicTable::GetElement("O")->getPrincipalIsotope()->getAtomicMass();

        m1 = Math::Round(static_cast<double>(m1), 9, MidpointRounding::AwayFromZero);

        double m2 = ye.back().MonoisotopicMass;
        double m3 = m1 - m2;

        Assert::IsTrue(m3 < 1e-9);

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
        delete prot;
    }

    void TestProteinDigestion::TestPeptideWithFixedModifications()
    {
        auto prot = new Protein("M", "");
        DigestionParams *digestionParams = new DigestionParams("trypsin", 0, 1, int::MaxValue, 1024, InitiatorMethionineBehavior::Variable, 3, CleavageSpecificity::Full, FragmentationTerminus::Both); // if you pass Custom Protease7 this test gets really flakey.
        std::vector<Modification*> fixedMods;
        ModificationMotif motif;
        ModificationMotif::TryGetMotif("M", motif);

        Modification tempVar("ProtNmod", "", "", "", motif, "N-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        fixedMods.push_back(&tempVar);
        Modification tempVar2("pepNmod", "", "", "", motif, "Peptide N-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        fixedMods.push_back(&tempVar2);
        Modification tempVar3("resMod", "", "", "", motif, "Anywhere.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        fixedMods.push_back(&tempVar3);
        Modification tempVar4("PepCmod", "", "", "", motif, "Peptide C-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        fixedMods.push_back(&tempVar4);
        Modification tempVar5("ProtCmod", "", "", "", motif, "C-terminal.", ChemicalFormula::ParseFormula("H"), std::make_optional(PeriodicTable::GetElement(1)->getPrincipalIsotope()->getAtomicMass()), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        fixedMods.push_back(&tempVar5);

        auto ok = prot->Digest(digestionParams, fixedMods, std::vector<Modification*>()).ToList();

        Assert::AreEqual(1, ok.size());

        Assert::AreEqual("[:pepNmod on M]M[:resMod on M][:ProtCmod on M]", ok.front().FullSequence);

        Assert::AreEqual("[H]M[H][H]", ok.front().SequenceWithChemicalFormulas);
        Assert::AreEqual(5 * PeriodicTable::GetElement("H")->getPrincipalIsotope()->getAtomicMass() + Residue::ResidueMonoisotopicMass['M'] + PeriodicTable::GetElement("O")->getPrincipalIsotope()->getAtomicMass(), ok.back().MonoisotopicMass, 1e-9);

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
        delete prot;
    }

    void TestProteinDigestion::TestDigestIndices()
    {
        ModificationMotif motifN;
        ModificationMotif::TryGetMotif("N", motifN);
        ModificationMotif motifR;
        ModificationMotif::TryGetMotif("R", motifR);
        Modification *modN = new Modification("myMod", "", "myModType", "", motifN, "Anywhere.", nullptr, std::make_optional(10), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        Modification *modR = new Modification("myMod", "", "myModType", "", motifR, "Anywhere.", nullptr, std::make_optional(10), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        std::unordered_map<int, std::vector<Modification*>> modDict =
        {
            {
                2, {modN}
            },
            {
                8, {modR}
            }
        };
        auto prot = new Protein("MNNNNKRRRRR", "", "", std::vector<std::tuple<std::string, std::string>>(), modDict, std::vector<ProteolysisProduct>(), "", "", true, false, std::vector<DatabaseReference>(), std::vector<SequenceVariation>(), std::vector<SequenceVariation>(), "", std::vector<DisulfideBond>(), std::vector<SpliceSite>(), "");

        DigestionParams *digestionParams = new DigestionParams("trypsin", 2, 5, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);

        auto digestedList = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();
        auto ok1 = digestedList[1];
        auto ok2 = digestedList[3];

        Assert::AreEqual(1, ok1.NumMods);
        Assert::IsTrue(ok1.AllModsOneIsNterminus->ContainsKey(3));
        Assert::AreEqual(1, ok2.NumMods);
        Assert::IsTrue(ok2.AllModsOneIsNterminus->ContainsKey(3));

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
        delete prot;
        delete modR;
        delete modN;
    }

    void TestProteinDigestion::TestDigestDecoy()
    {
        ModificationMotif motifN;
        ModificationMotif::TryGetMotif("N", motifN);
        ModificationMotif motifR;
        ModificationMotif::TryGetMotif("R", motifR);
        Modification *modN = new Modification("myMod", "", "myModType", "", motifN, "Anywhere.", nullptr, std::make_optional(10), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        Modification *modR = new Modification("myMod", "", "myModType", "", motifR, "Anywhere.", nullptr, std::make_optional(10), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        std::unordered_map<int, std::vector<Modification*>> modDict =
        {
            {
                2, {modN}
            },
            {
                8, {modR}
            }
        };
        auto prot = new Protein("MNNNNKRRRRR", "", "", std::vector<std::tuple<std::string, std::string>>(), modDict, std::vector<ProteolysisProduct>(), "", "", true, false, std::vector<DatabaseReference>(), std::vector<SequenceVariation>(), std::vector<SequenceVariation>(), "", std::vector<DisulfideBond>(), std::vector<SpliceSite>(), "");

        DigestionParams *digestionParams = new DigestionParams("trypsin", 2, 5, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);

        auto digestedList = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();
        auto ok1 = digestedList[1];
        auto ok2 = digestedList[3];

        Assert::AreEqual(1, ok1.NumMods);
        Assert::IsTrue(ok1.AllModsOneIsNterminus->ContainsKey(3));
        Assert::AreEqual(1, ok2.NumMods);
        Assert::IsTrue(ok2.AllModsOneIsNterminus->ContainsKey(3));

        prot = new Protein("MNNNNKRRRRR", "", "", std::vector<std::tuple<std::string, std::string>>(), modDict);
        ok1 = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).front();
        ok2 = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).back();

        Assert::AreEqual(0, ok1.NumMods);
        Assert::IsFalse(ok1.AllModsOneIsNterminus::Any());
        Assert::AreEqual(2, ok2.NumMods);
        Assert::IsTrue(ok2.AllModsOneIsNterminus::Any());

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
        delete prot;
        delete modR;
        delete modN;
    }

    void TestProteinDigestion::TestGoodPeptideWithLength()
    {
        auto prot = new Protein("MNNNKQQQQMNNNKQQQQ", "");

        DigestionParams *digestionParams = new DigestionParams("trypsin", 0, 1, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        auto ye = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();
        digestionParams = new DigestionParams("trypsin", 0, 5, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        auto ye1 = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();
        digestionParams = new DigestionParams("trypsin", 0, 1, 5, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        auto ye2 = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();
        digestionParams = new DigestionParams("trypsin", 0, 5, 8, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        auto ye3 = prot->Digest(digestionParams, std::vector<Modification*>(), std::vector<Modification*>()).ToList();
        Assert::AreEqual(3, ye.size());
        Assert::AreEqual(2, ye1.size());
        Assert::AreEqual(2, ye2.size());
        Assert::AreEqual(1, ye3.size());

//C# TO C++ CONVERTER TODO TASK: A 'delete digestionParams' statement was not added since digestionParams was passed to a method or constructor. Handle memory management manually.
        delete prot;
    }

    void TestProteinDigestion::Test_ProteinDigest()
    {
        DigestionParams *d = new DigestionParams("trypsin", 0, 5, int::MaxValue, 1024, InitiatorMethionineBehavior::Retain, 2, CleavageSpecificity::Full, FragmentationTerminus::Both);
        ModificationMotif motif;
        ModificationMotif::TryGetMotif("D", motif);
        Modification *mod = new Modification("mod1", "", "mt", "", motif, "Anywhere.", nullptr, std::make_optional(10), std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");

        std::unordered_map<int, std::vector<Modification*>> oneBasedModification =
        {
            {
                3, {mod}
            }
        };

        Protein *prot1 = new Protein("MEDEEK", "prot1", "", std::vector<std::tuple<std::string, std::string>>(), oneBasedModification, std::vector<ProteolysisProduct>(), "", "", false, false, std::vector<DatabaseReference>(), std::vector<SequenceVariation>(), std::vector<SequenceVariation>(), "", std::vector<DisulfideBond>(), std::vector<SpliceSite>(), "");

        auto pep1 = prot1->Digest(d, std::vector<Modification*>(), std::vector<Modification*>()).front();
        auto pep2 = prot1->Digest(d, std::vector<Modification*>(), std::vector<Modification*>()).back();

        Assert::AreEqual("MEDEEK", pep1->FullSequence);
        Assert::AreEqual("MED[mt:mod1 on D]EEK", pep2->FullSequence);

        delete prot1;
        delete mod;
//C# TO C++ CONVERTER TODO TASK: A 'delete d' statement was not added since d was passed to a method or constructor. Handle memory management manually.
    }

    void TestProteinDigestion::TestReadPeptideFromString()
    {
        // set up the test

        ModificationMotif target;
        ModificationMotif::TryGetMotif("T", target);

        Modification *carbamidomethylOnC = new Modification("Carbamidomethyl on C", "", "Common Fixed", "", target, "Unassigned.", ChemicalFormula::ParseFormula("C2H3NO"), std::nullopt, std::unordered_map<std::string, std::vector<std::string>>(), std::unordered_map<std::string, std::vector<std::string>>(), std::vector<std::string>(), std::unordered_map<DissociationType, std::vector<double>>(), std::unordered_map<DissociationType, std::vector<double>>(), "");
        std::string sequence = "HQVC[Common Fixed:Carbamidomethyl on C]TPGGTTIAGLC[Common Fixed:Carbamidomethyl on C]VMEEK";

        // parse the peptide from the string
        PeptideWithSetModifications *peptide = new PeptideWithSetModifications(sequence, std::unordered_map<std::string, Modification*>
        {
            {carbamidomethylOnC->getIdWithMotif(), carbamidomethylOnC}
        });

        // test base sequence and full sequence
        Assert::That(peptide->getBaseSequence() == "HQVCTPGGTTIAGLCVMEEK");
        Assert::That(peptide->getFullSequence() == sequence);

        // test peptide mass
        Assert::That(std::round(peptide->getMonoisotopicMass() * std::pow(10, 5)) / std::pow(10, 5) == 2187.01225);

        // test mods (correct id, correct number of mods, correct location of mods)
        Assert::That(peptide->getAllModsOneIsNterminus().First()->Value->IdWithMotif == "Carbamidomethyl on C");
        Assert::That(peptide->getAllModsOneIsNterminus().size() == 2);
        Assert::That((std::unordered_set<int>(peptide->getAllModsOneIsNterminus().Keys))->SetEquals(std::unordered_set<int> {5, 16}));

        // calculate fragments. just check that they exist and it doesn't crash
        std::vector<Product*> theoreticalFragments = peptide->Fragment(DissociationType::HCD, FragmentationTerminus::Both).ToList();
        Assert::That(theoreticalFragments.size() > 0);

        delete peptide;
//C# TO C++ CONVERTER TODO TASK: A 'delete carbamidomethylOnC' statement was not added since carbamidomethylOnC was passed to a method or constructor. Handle memory management manually.
    }
}