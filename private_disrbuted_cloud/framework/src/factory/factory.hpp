#pragma once

#include <iostream>
#include <unordered_map>
#include <functional>
#include <memory>

// Factory class template
namespace ilrd
{

// Factory class template
template<typename BASE,typename KEY, typename PARAM>				
class Factory 
{
public:
	
    template<typename DERIVED>
    void Add(const KEY& key);
	std::shared_ptr<BASE> Create(const KEY& key, PARAM param);   

private:
    template<typename DERIVED>
    static BASE *Creator(PARAM param);
	
    std::unordered_map<KEY, BASE* (*)(PARAM param)> m_creators; 
     			
};

template<typename BASE,typename KEY, typename PARAM>						
    template<typename DERIVED>
    void Factory<BASE, KEY, PARAM>::Add(const KEY& key)
    {
        m_creators[key] = &Factory<BASE, KEY, PARAM>::Creator<DERIVED>;
    }


template<typename BASE,typename KEY, typename PARAM>				
     std::shared_ptr<BASE> Factory<BASE, KEY, PARAM>::Create(const KEY& key, PARAM param)
     {
        return std::shared_ptr<BASE>(m_creators[key](param));
     }

template<typename BASE,typename KEY, typename PARAM>
template<typename DERIVED>
BASE *Factory<BASE, KEY, PARAM>::Creator(PARAM param)
{
    return new DERIVED(param);
}

} //namespace ilrd