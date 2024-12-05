# -*- coding: utf-8 -*-

from sst_unittest import *
from sst_unittest_support import *


class testcase_miranda_Component(SSTTestCase):

    def setUp(self):
        super(type(self), self).setUp()
        # Put test based setup code here. it is called once before every test

    def tearDown(self):
        # Put test based teardown code here. it is called once after every test
        super(type(self), self).tearDown()

#####
    Spatter_missing = not sst_elements_config_include_file_get_value(define="HAVE_SPATTER", type=int, default=0, disable_warning=True)

    @unittest.skipIf(Spatter_missing, "test_miranda_spatterbench test: Requires Spatter, but Spatter is not found in build configuration.")
    def test_miranda_spatterbench_gather(self):
        stat_file = "test_miranda_spatterbench_gather_stats.csv"
        model_options = "-pUNIFORM:8:1 -kgather -l" + str(2**16) + " --statfile={0}".format(stat_file)
        self.miranda_test_template("spatterbench_gather", "spatterbench.py", "--model-options=\"{0}\"".format(model_options))

    @unittest.skipIf(Spatter_missing, "test_miranda_spatterbench test: Requires Spatter, but Spatter is not found in build configuration.")
    def test_miranda_spatterbench_scatter(self):
        stat_file = "test_miranda_spatterbench_scatter_stats.csv"
        model_options = "-pUNIFORM:8:1 -kscatter -l" + str(2**16) + " --statfile={0}".format(stat_file)
        self.miranda_test_template("spatterbench_scatter", "spatterbench.py", "--model-options=\"{0}\"".format(model_options))

    @unittest.skipIf(Spatter_missing, "test_miranda_spatterbench test: Requires Spatter, but Spatter is not found in build configuration.")
    def test_miranda_spatterbench_gs(self):
        stat_file = "test_miranda_spatterbench_gs_stats.csv"
        model_options = "-gUNIFORM:8:1 -uUNIFORM:8:1 -kgs -l" + str(2**16) + " --statfile={0}".format(stat_file)
        self.miranda_test_template("spatterbench_gs", "spatterbench.py", "--model-options=\"{0}\"".format(model_options))

    @unittest.skipIf(Spatter_missing, "test_miranda_spatterbench test: Requires Spatter, but Spatter is not found in build configuration.")
    def test_miranda_spatterbench_multiscatter(self):
        stat_file = "test_miranda_spatterbench_multiscatter_stats.csv"
        model_options = "-pUNIFORM:8:1 -uUNIFORM:8:1 -kmultiscatter -l" + str(2**16) + " --statfile={0}".format(stat_file)
        self.miranda_test_template("spatterbench_multiscatter", "spatterbench.py", "--model-options=\"{0}\"".format(model_options))

    @unittest.skipIf(Spatter_missing, "test_miranda_spatterbench test: Requires Spatter, but Spatter is not found in build configuration.")
    def test_miranda_spatterbench_multigather(self):
        stat_file = "test_miranda_spatterbench_multigather_stats.csv"
        model_options = "-pUNIFORM:8:1 -gUNIFORM:8:1 -kmultigather -l" + str(2**16) + " --statfile={0}".format(stat_file)
        self.miranda_test_template("spatterbench_multigather", "spatterbench.py", "--model-options=\"{0}\"".format(model_options))

#####

    def miranda_test_template(self, testcase, sdlfile, otherargs="", testtimeout=240):
        # Get the path to the test files
        test_path = self.get_testsuite_dir()
        outdir = self.get_test_output_run_dir()
        tmpdir = self.get_test_output_tmp_dir()

        # Set the various file paths
        testDataFileName="test_miranda_{0}".format(testcase)

        sdlfile = "{0}/{1}".format(test_path, sdlfile)
        reffile = "{0}/refFiles/{1}.out".format(test_path, testDataFileName)
        outfile = "{0}/{1}.out".format(outdir, testDataFileName)
        errfile = "{0}/{1}.err".format(outdir, testDataFileName)
        mpioutfiles = "{0}/{1}.testfile".format(outdir, testDataFileName)

        refstatfilecsv = "{0}/refFiles/{1}_stats.csv".format(test_path, testDataFileName)
        outstatfilecsv = "{0}/{1}_stats.csv".format(outdir, testDataFileName)

        self.run_sst(sdlfile, outfile, errfile, mpi_out_files=mpioutfiles, other_args=otherargs, timeout_sec=testtimeout)

        testing_remove_component_warning_from_file(outfile)

        # NOTE: THE PASS / FAIL EVALUATIONS ARE PORTED FROM THE SQE BAMBOO
        #       BASED testSuite_XXX.sh THESE SHOULD BE RE-EVALUATED BY THE
        #       DEVELOPER AGAINST THE LATEST VERSION OF SST TO SEE IF THE
        #       TESTS & RESULT FILES ARE STILL VALID

        # Perform the tests
        if os_test_file(errfile, "-s"):
            log_testing_note("miranda test {0} has a Non-Empty Error File {1}".format(testDataFileName, errfile))

        cmp_result = testing_compare_sorted_diff(testcase, outfile, reffile)
        if (cmp_result == False):
            diffdata = testing_get_diff_data(testcase)
            log_failure(diffdata)
        self.assertTrue(cmp_result, "Sorted Output file {0} does not match sorted Reference File {1}".format(outfile, reffile))

        cmp_result = testing_compare_sorted_diff(testcase, outstatfilecsv, refstatfilecsv)
        if (cmp_result == False):
            diffdata = testing_get_diff_data(testcase)
            log_failure(diffdata)
        self.assertTrue(cmp_result, "Sorted Output file {0} does not match sorted Reference File {1}".format(outstatfilecsv, refstatfilecsv))
