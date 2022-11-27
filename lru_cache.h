/*
 *  Created on: 7 November, 2020
 *      Author: Lior Sinai
 * Description: a generic Least Recently Used Cache
 *     Sources: https://bhrigu.me/blog/2017/01/22/lru-cache-c-plus-plus-implementation/
 *              https://www.geeksforgeeks.org/lru-cache-implementation/
 * 
 *  Values stored in queue -> doubly linked between prev and next
 *  Use hashmap to find place in queue
 * 
 */

#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <memory>

template <typename keyType, typename  valType>
class Node {
  public:
  keyType key; // this is to help with debugging. It is not used for processing
  valType value;
  Node *prev, *next;
  Node(keyType k, valType v): key(k), value(v), prev(NULL), next(NULL) {}
};

template <typename keyType, typename  valType>
class DoublyLinkedList {
private:
  Node<keyType, valType> *front, *back;
  bool isEmpty();

public:
  DoublyLinkedList(): front(NULL), back(NULL) {}
  Node<keyType, valType>* add_page_to_head(keyType key, valType value);
  void move_page_to_head(Node<keyType, valType>* page);
  void remove_back();
  Node<keyType, valType>* get_back();
};

template <typename  keyType, typename  valType> 
class LRUCache{
private:
  int max_size, size;
  DoublyLinkedList<keyType, valType> *pageQueue;
  std::unordered_map<keyType, Node<keyType, valType>*> pageMap;

  public:
    int hits{0};    int misses{0};
    LRUCache(int max_size = 1000);
    bool exists(keyType key);
    valType get(keyType key);
    void put(keyType key, valType value);
    void info();
    ~LRUCache();
};


/* --------------------- Function definitions  -------------------- */
// Moved here because otherwise the linker gives errors because of the template

/* ---------- DoublyLinkedList functions ---------- */
template<class keyType, class valType> 
bool DoublyLinkedList<keyType, valType>::isEmpty(){
    return this->back == NULL;
}

template<class keyType, class valType> 
Node<keyType, valType>* DoublyLinkedList<keyType, valType>::add_page_to_head(keyType key, valType value){
    Node<keyType, valType> *page = new Node<keyType, valType>(key, value);
      if(!front && !back) {
          front = back = page;
      }
      else {
          page->next = front;
          front->prev = page;
          front = page;
      }
      return page;
}

template<class keyType, class valType> 
void DoublyLinkedList<keyType, valType>::move_page_to_head(Node<keyType, valType>* page){
    if(page==front) {
        return;
    }
    if(page == back) {
        back = back->prev;
        back->next = NULL;
    }
    else{ //this node is now skipped here
        page->prev->next = page->next;
        page->next->prev = page->prev;
    }
    // set values for this node
    page->next = front;
    page->prev = NULL;
    // set values for second place (old front)
    front->prev = page;
    // redefine front
    front = page;
}

template<class keyType, class valType> 
void DoublyLinkedList<keyType, valType>::remove_back(){
    if(this->isEmpty()) {
          return;
      }
      if(front == back) {
          delete back;
          front = back = NULL;
      }
      else {
          Node<keyType, valType> *temp = back;
          back = back->prev;
          back->next = NULL;
          delete temp;
      }
}

template<class keyType, class valType> 
Node<keyType, valType>* DoublyLinkedList<keyType, valType>::get_back(){
    return back;
}

/* ---------- LRU Cache functions ---------- */
template<class keyType, class valType> 
LRUCache<keyType, valType>::LRUCache(int max_size){
    this->max_size = max_size;
    size = 0;
    pageQueue = new DoublyLinkedList<keyType, valType>();
    pageMap = std::unordered_map<keyType, Node<keyType, valType>*>();
}

template<class keyType, class valType> 
bool LRUCache<keyType,  valType>::exists(keyType key){
    return pageMap.find(key)!=pageMap.end();
}

template<class keyType, class valType> 
valType LRUCache<keyType,  valType>::get(keyType key){
    if(pageMap.find(key)==pageMap.end()) {
        throw std::out_of_range ("key not found");
    }
    std::hash<std::string> hasher;
    size_t hash = hasher(key);

    valType val = pageMap[key]->value;

    // move the page to front
    pageQueue->move_page_to_head(pageMap[key]);
    return val;
}

template<class keyType, class valType> 
void LRUCache<keyType,  valType>::put(keyType key, valType value){
    if(pageMap.find(key)!=pageMap.end()) {
            // if key already present, update value and move page to head
            pageMap[key]->value = value;
            pageQueue->move_page_to_head(pageMap[key]);
            return;
        }
    if(size == this->max_size) {
        // remove rear page
        keyType k = pageQueue->get_back()->key;
        pageMap.erase(k);
        pageQueue->remove_back();
        size--;
    }

    // add new page to head to Queue
    Node<keyType, valType> *page = pageQueue->add_page_to_head(key, value);
    size++;
    pageMap[key] = page;
}

template<class keyType, class valType> 
LRUCache<keyType,  valType>::~LRUCache() {
    typename std::unordered_map<keyType, Node<keyType, valType>*>::iterator i1;
    for(i1=pageMap.begin(); i1!=pageMap.end(); i1++) {
        delete i1->second;
    }
    delete pageQueue;
}

template<class keyType, class valType> 
void LRUCache<keyType,  valType>::info(){
    float ratio = misses > 0 ? (float)hits/(float)misses : 0;
    std::cout << "LRU cache info -- " << 
                "hits/misses: " <<ratio <<
                 ", hits: " << hits <<
                 ", misses: " << misses <<
                 ", size: " << size <<
                 ", max_size: " << max_size << std::endl;
}

#endif //LRU_CACHE_H