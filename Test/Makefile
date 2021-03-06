#
# Copyright (c) 2019      University of Houston. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#
include ../Makefile.defs

CEXECS = TestChemicalFormula	   	     \
	TestElementsAndIsotopes		     \
	TestAminoAcids			     \
	TestModifications		     \
	TestFragments	 		     \
	TestDigestionMotif                   \
	TestMetaMorpheus                     \
	TestSpectra                          \
	TestProteinDigestion                 \
	TestProteinProperties                \
	TestPeptides			     \
	TestPtmListLoader                    \
	TestModFits                          \
	TestChromatogram                     \
	TestMgf                              \
	TestMzML

#	FakeMsDataFile                       \
#	FixtureSetUp                         \
#	SpectrumProcessingAndFiltering       \
#	TestIsolation                        \
#	TestMassMzCalculations               \
#	TestMsDataFile                       \
#	TestPeptideWithSetMods               \
#	TestProductMassesMightHaveDuplicates \
#	TestRangeAndTolerances               \
#	TestRetentionTimePrediction          \
#	TestSeqCoverage                      \
#	TestVariantProtein


.SUFFIXES: .cxx .h  

###################################################################################################

all: cxxtests 

cxxtests: $(CEXECS)


###################################################################################################
### CXXTESTS                                                                                      ###
###################################################################################################
TestElementsAndIsotopes: TestElementsAndIsotopes.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestChemicalFormula: TestChemicalFormula.o
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestAminoAcids: TestAminoAcids.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestModifications: TestModifications.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestPeptides: TestPeptides.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestDigestionMotif: TestDigestionMotif.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestFragments: TestFragments.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestMetaMorpheus: TestMetaMorpheus.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestProteinProperties: TestProteinProperties.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestProteinDigestion: TestProteinDigestion.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestSpectra: TestSpectra.o 
	$(CXX) $^ -o $@ $(LDFLAGS)
#
TestPtmListLoader: TestPtmListLoader.o 
	$(CXX) $^ -o $@ $(LDFLAGS)

TestPeptideWithSetMods: TestPeptideWithSetMods.o
	$(CXX) $^ -o $@ $(LDFLAGS)

TestModFits: TestModFits.o
	$(CXX) $^ -o $@ $(LDFLAGS)

TestChromatogram: TestChromatogram.o
	$(CXX) $^ -o $@ $(LDFLAGS)

TestMgf: TestMgf.o
	$(CXX) $^ -o $@ $(LDFLAGS)

TestMzML: TestMzML.o 
	$(CXX) $(CXXFLAGS) -c FakeMsDataFile.cpp
	$(CXX) $^ -o $@ FakeMsDataFile.o $(LDFLAGS)

###################################################################################################


clean:
	cd DatabaseTests ; make clean ; cd ..
	cd TestFlashLFQ ; make clean ; cd ..
	rm -rf *.o *~

	rm -f TestChemicalFormula TestElementsAndIsotopes TestDigestionMotif
	rm -f TestPeptides TestAminoAcids TestModifications TestFragments TestMetaMorpheus
	rm -f TestProteinDigestion TestProteinProperties TestSpectra TestMzML TestMgf
	rm -f TestPeptideWithSetMods TestPtmListLoader TestModFits TestChromatogram
