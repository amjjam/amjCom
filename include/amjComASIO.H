#ifndef _AMJCOMASIO_H_
#define _AMJCOMASIO_H_

#include <asio.hpp>
#include <iostream>

namespace amjCom{
  class IOContext{
  public:
    IOContext(const IOContext &other){
      _io_context_ptr=other._io_context_ptr;
      std::cout << "amjCom::IOContext: copy, count=" << _io_context_ptr.use_count() << std::endl;
    };
    IOContext(int n=1);
    ~IOContext(){std::cout << "amjCom::IOContext: destroy, count=" << _io_context_ptr.use_count() << std::endl;};
    asio::io_context &io_context(){return *_io_context_ptr;};
    void post(std::function<void()> f){_io_context_ptr->post(f);};
  private:
    std::shared_ptr<asio::io_context> _io_context_ptr;
  };
}

#endif /* _AMJCOMASIO_H_ */
