%module core_testpy

%include "Core/array_typemaps.i"
%import "Core/core.i"
//===========================================================================
// Some test functions. TODO: move to some test-lib?
//===========================================================================

%inline %{

struct ListTest {
  double d;
  ~ListTest() {};
  ListTest(double d) : d(d) {};
  ListTest() {};
};

%}

%extend ListTest {
%pythoncode %{
def __str__(self):
    return "ListTest: d = " + str(self.d)

def __repr__(self):
    return "<swig_wrapped ListTest: d = " + str(self.d) + ">"
%}
}

%List_Typemap(ListTest)

%inline %{

struct TestClass {
  MT::Array<double> a_val;
  MT::Array<double>* a_poi;

  MT::Array<ListTest*> a_list;

  TestClass() : a_val({1.2, 3.4}), a_poi(new arr({9.0, 1.2})) {};

  MT::Array<double> get_value() { return a_val; }
  MT::Array<double>* get_pointer() { return a_poi; }
};

%}


%inline %{

MT::Array<double> identity_arr_value(MT::Array<double> INPUT) {
  return INPUT;
}

const MT::Array<double> identity_const_arr_value(const MT::Array<double> INPUT) {
  return INPUT;
}

MT::Array<double> identity_arr_reference(MT::Array<double> &INPUT) {
  return INPUT;
}

const MT::Array<double> identity_const_arr_reference(const MT::Array<double> &INPUT) {
  return INPUT;
}

MT::Array<double> identity_arr_pointer(MT::Array<double> *INPUT) {
  return *INPUT;
}

const MT::Array<double> identity_const_arr_pointer(const MT::Array<double> *INPUT) {
  return *INPUT;
}

MT::Array<int> identity_intA_value(MT::Array<int> INPUT) {
  return INPUT;
}

MT::Array<int> identity_intA_reference(MT::Array<int> &INPUT) {
  return INPUT;
}

MT::Array<int> identity_intA_pointer(MT::Array<int> *INPUT) {
  return *INPUT;
}

MT::Array<uint> identity_uintA_value(MT::Array<uint> INPUT) {
  return INPUT;
}

MT::Array<uint> identity_uintA_reference(MT::Array<uint> &INPUT) {
  return INPUT;
}

MT::Array<uint> identity_uintA_pointer(MT::Array<uint> *INPUT) {
  return *INPUT;
}

typedef MT::Array<double> arr;

const arr test_typedefs(const arr& a) {
  return a;
}

const arr test_overloading(double x) {
  return {x};
}

const arr test_overloading(arr& x) {
  return x;
}

const arr test_overloading(arr& x, double y) {
  return y*x;
}

MT::Array<double> return_arr() {
  arr t = { 1.2, 3.4, 5.6, 7.8};
  t.reshape(2,2);
  return t;
}

MT::Array<int> return_intA() {
  intA t = { -1, 3, -5, 7};
  t.reshape(2,2);
  return t;
}

MT::Array<uint> return_uintA() {
  uintA t = { 1, 3, 5, 7};
  t.reshape(2,2);
  return t;
}

arrL return_arrL() {
  arr *a = new arr({1.2, 3.4, 5.6, 7.8});
  arrL l;
  l.append(a);
  return l;
}

arrL identity_arrL_value(arrL in) {
  return in;
}

arrL identity_arrL_reference(arrL& in) {
  return in;
}

arrL identity_arrL_pointer(arrL* in) {
  return *in;
}

MT::Array<ListTest*> id_list_test(MT::Array<ListTest*> a) {
  return a;  
}

void argout_list_test(MT::Array<ListTest*> &l) {
  ListTest* t1 = new ListTest(10);
  ListTest* t2 = new ListTest(20);
  l.resize(2);
  l(0) = t1;
  l(1) = t2;
}

void argout_inplace_list_test(MT::Array<ListTest*> &l) {
  if(l.N != 2) { return; }
  ListTest *tmp = l(0);
  l(0) = l(1);
  l(1) = tmp;
}

void argout_test(MT::Array<double>& io) {
  io = {1.2, 3.4};
}

void multi_argout_test(MT::Array<double>& io1, MT::Array<double>& io2) {
  io1 = {1.2, 3.4};
  io2 = {5.6, 7.8};
}

int argout_test_plus_return(MT::Array<double>& io) {
  io = {1.2, 3.4};
  return 1;
}

int argout_test_mixed(double d, MT::Array<double>& io1, int a, MT::Array<double>& io2) {
  io1 = {1.2, 3.4};
  io2 = {5.6, 7.8};
  return 1;
}


%}  

//===========================================================================
