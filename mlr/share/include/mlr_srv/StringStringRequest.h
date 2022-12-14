// Generated by gencpp from file mlr_srv/StringStringRequest.msg
// DO NOT EDIT!


#ifndef MLR_SRV_MESSAGE_STRINGSTRINGREQUEST_H
#define MLR_SRV_MESSAGE_STRINGSTRINGREQUEST_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace mlr_srv
{
template <class ContainerAllocator>
struct StringStringRequest_
{
  typedef StringStringRequest_<ContainerAllocator> Type;

  StringStringRequest_()
    : str()  {
    }
  StringStringRequest_(const ContainerAllocator& _alloc)
    : str(_alloc)  {
    }



   typedef std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other >  _str_type;
  _str_type str;




  typedef boost::shared_ptr< ::mlr_srv::StringStringRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::mlr_srv::StringStringRequest_<ContainerAllocator> const> ConstPtr;

}; // struct StringStringRequest_

typedef ::mlr_srv::StringStringRequest_<std::allocator<void> > StringStringRequest;

typedef boost::shared_ptr< ::mlr_srv::StringStringRequest > StringStringRequestPtr;
typedef boost::shared_ptr< ::mlr_srv::StringStringRequest const> StringStringRequestConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::mlr_srv::StringStringRequest_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::mlr_srv::StringStringRequest_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace mlr_srv

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'geometry_msgs': ['/opt/ros/indigo/share/geometry_msgs/msg'], 'std_msgs': ['/opt/ros/indigo/share/std_msgs/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::mlr_srv::StringStringRequest_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::mlr_srv::StringStringRequest_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::mlr_srv::StringStringRequest_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "994972b6e03928b2476860ce6c4c8e17";
  }

  static const char* value(const ::mlr_srv::StringStringRequest_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x994972b6e03928b2ULL;
  static const uint64_t static_value2 = 0x476860ce6c4c8e17ULL;
};

template<class ContainerAllocator>
struct DataType< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "mlr_srv/StringStringRequest";
  }

  static const char* value(const ::mlr_srv::StringStringRequest_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
{
  static const char* value()
  {
    return "string str\n\
";
  }

  static const char* value(const ::mlr_srv::StringStringRequest_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.str);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER;
  }; // struct StringStringRequest_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::mlr_srv::StringStringRequest_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::mlr_srv::StringStringRequest_<ContainerAllocator>& v)
  {
    s << indent << "str: ";
    Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.str);
  }
};

} // namespace message_operations
} // namespace ros

#endif // MLR_SRV_MESSAGE_STRINGSTRINGREQUEST_H
