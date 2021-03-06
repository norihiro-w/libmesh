// Ignore unused parameter warnings coming from cppunit headers
#include <libmesh/ignore_warnings.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <libmesh/restore_warnings.h>

#include <libmesh/equation_systems.h>
#include <libmesh/mesh.h>
#include <libmesh/mesh_generation.h>
#include <libmesh/serial_mesh.h>

#include "test_comm.h"

using namespace libMesh;

Number cubic_test (const Point& p,
                   const Parameters&,
                   const std::string&,
                   const std::string&)
{
  const Real & x = p(0);
  const Real & y = p(1);
  const Real & z = p(2);
  
  return x*(1-x)*(1-x) + x*x*(1-y) + x*(1-y)*(1-z) + y*(1-y)*z + z*(1-z)*(1-z);
}


class SystemsTest : public CppUnit::TestCase {
public:
  CPPUNIT_TEST_SUITE( SystemsTest );

  CPPUNIT_TEST( testProjectHierarchicEdge3 );
  CPPUNIT_TEST( testProjectHierarchicQuad9 );
  CPPUNIT_TEST( testProjectHierarchicTri6 );
  CPPUNIT_TEST( testProjectHierarchicHex27 );

  CPPUNIT_TEST_SUITE_END();

private:

public:
  void setUp()
  {}

  void tearDown()
  {}



  void testProjectLine(const ElemType elem_type)
  {
    Mesh mesh(*TestCommWorld);

    EquationSystems es(mesh);
    System &sys = es.add_system<System> ("SimpleSystem");
    sys.add_variable("u", THIRD, HIERARCHIC);

    MeshTools::Generation::build_line (mesh,
                                       3,
                                       0., 1.,
                                       elem_type);

    es.init();
    sys.project_solution(cubic_test, NULL, es.parameters);

    for (Real x = 0.1; x < 1; x += 0.2)
      {
        Point p(x);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(libmesh_real(sys.point_value(0,p)),
                                     libmesh_real(cubic_test(p,es.parameters,"","")),
                                     TOLERANCE*TOLERANCE);
      }
  }



  void testProjectSquare(const ElemType elem_type)
  {
    Mesh mesh(*TestCommWorld);

    EquationSystems es(mesh);
    System &sys = es.add_system<System> ("SimpleSystem");
    sys.add_variable("u", THIRD, HIERARCHIC);

    MeshTools::Generation::build_square (mesh,
                                         3, 3,
                                         0., 1., 0., 1.,
                                         elem_type);

    es.init();
    sys.project_solution(cubic_test, NULL, es.parameters);

    for (Real x = 0.1; x < 1; x += 0.2)
      for (Real y = 0.1; y < 1; y += 0.2)
        {
          Point p(x,y);
          CPPUNIT_ASSERT_DOUBLES_EQUAL(libmesh_real(sys.point_value(0,p)),
                                       libmesh_real(cubic_test(p,es.parameters,"","")),
                                       TOLERANCE*TOLERANCE);
        }
  }


  void testProjectCube(const ElemType elem_type)
  {
    Mesh mesh(*TestCommWorld);

    EquationSystems es(mesh);
    System &sys = es.add_system<System> ("SimpleSystem");
    sys.add_variable("u", THIRD, HIERARCHIC);

    MeshTools::Generation::build_cube (mesh,
                                       3, 3, 3,
                                       0., 1., 0., 1., 0., 1.,
                                       elem_type);

    es.init();
    sys.project_solution(cubic_test, NULL, es.parameters);

    for (Real x = 0.1; x < 1; x += 0.2)
      for (Real y = 0.1; y < 1; y += 0.2)
        for (Real z = 0.1; z < 1; z += 0.2)
          {
            Point p(x,y,z);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(libmesh_real(sys.point_value(0,p)),
                                         libmesh_real(cubic_test(p,es.parameters,"","")),
                                         TOLERANCE*TOLERANCE);
          }
  }

  void testProjectHierarchicEdge3() { testProjectLine(EDGE3); }
  void testProjectHierarchicQuad9() { testProjectSquare(QUAD9); }
  void testProjectHierarchicTri6()  { testProjectSquare(TRI6); }
  void testProjectHierarchicHex27() { testProjectCube(HEX27); }

};

CPPUNIT_TEST_SUITE_REGISTRATION( SystemsTest );
