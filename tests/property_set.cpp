#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>

/* Thanks to Björn Pollex on StackOverflow for the following code:
*/
struct cout_redirect {
    cout_redirect(std::streambuf * new_buffer)
        : old(std::cout.rdbuf(new_buffer))
    { }

    ~cout_redirect() {
        std::cout.rdbuf(old);
    }

private:
    std::streambuf * old;
};

//-----------------------------------------------
// HERE BEGINS THE TEST SUITE
//

#include <gpc/gui/property_set.hpp>

struct MyPropertySetTraits {
    typedef int key_t;
    typedef int value_t;
    static const int DEFAULT_KEY = 0;
    static const int DEFAULT_VALUE = 100;
    static int getDerivedValue(int base_val, int key) {
        return base_val + key;
    }
};

typedef gpc::gui::PropertySet<MyPropertySetTraits> MyPropertySet;

BOOST_AUTO_TEST_SUITE(TestSuite)

BOOST_AUTO_TEST_CASE(main_test)
{
    MyPropertySet props;

    //std::cout << "props[0] = " << props[0] << std::endl;

    // Default value
    BOOST_CHECK(props[0] == 100);

    // Derived values
    BOOST_CHECK(props[1] == 101);
    BOOST_CHECK(props[2] == 102);
    BOOST_CHECK(props[3] == 103);

    // Change value: must keep new value
    props.set(3, 130);
    BOOST_CHECK(props[3] == 130);

    // Clear value: must revert to computed value
    props.clear(3);
    BOOST_CHECK(props[3] == 103);
}

BOOST_AUTO_TEST_SUITE_END()
