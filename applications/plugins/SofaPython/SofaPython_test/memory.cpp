#include <gtest/gtest.h>

#include <SofaPython/PythonScriptController.h>
#include <SofaPython/PythonScriptFunction.h>

#include <sofa/simulation/graph/DAGSimulation.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa {



static unsigned s_nbAlloc;
class PythonTestAllocationCounter : public core::objectmodel::BaseObject
{
public:
    SOFA_CLASS(PythonTestAllocationCounter, core::objectmodel::BaseObject);
protected:
    PythonTestAllocationCounter() { s_nbAlloc++; }
    ~PythonTestAllocationCounter() { s_nbAlloc--; }
};

int PythonTestAllocationCounterClass = core::RegisterObject("A test component counting allocations")
        .add< PythonTestAllocationCounter >()
        ;
SOFA_DECL_CLASS(PythonTestAllocationCounter)







struct MemoryTest : public ::testing::Test
{
    MemoryTest()
    {
    }

    void run()
    {
        static const std::string scenePath = std::string(SOFAPYTHON_TEST_PYTHON_DIR)+std::string("/test_memory.py");

        simulation::Simulation* simulation;
        sofa::simulation::setSimulation(simulation = new sofa::simulation::graph::DAGSimulation());

        /// Scene creation
        simulation::Node::SPtr root =  simulation::getSimulation()->load ( scenePath.c_str() );

        component::controller::PythonScriptController *ctr = root->get<component::controller::PythonScriptController>();

        sofa::core::objectmodel::PythonScriptFunction pythonScriptFunctionADD(PyObject_GetAttrString(ctr->scriptControllerInstance(), "add"), false);
        sofa::core::objectmodel::PythonScriptFunction pythonScriptFunctionREMOVE(PyObject_GetAttrString(ctr->scriptControllerInstance(), "remove"), false);
        sofa::core::objectmodel::PythonScriptFunction pythonScriptFunctionADDSUB(PyObject_GetAttrString(ctr->scriptControllerInstance(), "addSub"), false);
        sofa::core::objectmodel::PythonScriptFunction pythonScriptFunctionREMOVESUB(PyObject_GetAttrString(ctr->scriptControllerInstance(), "removeSub"), false);

        pythonScriptFunctionADD(NULL, NULL);
        EXPECT_EQ( s_nbAlloc, 10u );

        pythonScriptFunctionADD(NULL, NULL);
        EXPECT_EQ( s_nbAlloc, 20u );

        pythonScriptFunctionADDSUB(NULL, NULL);
        EXPECT_EQ( s_nbAlloc, 30u );

        pythonScriptFunctionREMOVESUB(NULL, NULL);
        EXPECT_EQ( s_nbAlloc, 20u );

        pythonScriptFunctionREMOVE(NULL, NULL);
        EXPECT_EQ( s_nbAlloc, 0u );


        pythonScriptFunctionADD(NULL, NULL);
        simulation::getSimulation()->unload(root);
        EXPECT_EQ( s_nbAlloc, 0u );
    }


};

TEST_F( MemoryTest, MemoryTest)
{
    this->run();
}


}// namespace sofa
