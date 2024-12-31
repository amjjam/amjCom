#include "../include/amjComASIO.H"

namespace amjCom{
  IOContext::IOContext(int n){//:_io_context_ptr(new asio::io_context){
    if(_io_context_ptr){
      std::cout << "amjCom::IOContext: reusing existing io_contex, count=" << _io_context_ptr.use_count() << std::endl;
      return;
    }
    std::cout << "amjCom::IOContext: making new io_context" << std::endl;
    _io_context_ptr.reset(new(asio::io_context));
    asio::thread t([*this]{
      std::cout << "amjCom::IOContext: thread start, count=" << _io_context_ptr.use_count() << std::endl;
      asio::executor_work_guard<decltype(_io_context_ptr->get_executor())>
	work{_io_context_ptr->get_executor()};
      _io_context_ptr->run();
      std::cout << "amjCom::IOContext: thread end" << std::endl;
    });
  }
  
}
