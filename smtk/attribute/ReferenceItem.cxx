//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/attribute/ReferenceItem.h"
#include "smtk/attribute/ReferenceItemDefinition.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Resource.h"
#include "smtk/attribute/UnsetValueError.h"

#include "smtk/resource/Component.h"

#include <boost/variant.hpp>

#include <cassert>
#include <sstream>

namespace smtk
{

namespace attribute
{

/// Internally, ReferenceItems cache retrieved PersistentObjects to speed up
/// repeated calls to their contents. These PersistentObjects can be cached as
/// std::shared_ptr-s or std::weak_ptr-s as determined by the flag
/// "holdReference" in ReferenceItemDefinition. To accomplish this, we cache
/// retrieved values as a variant that accepts both types.
struct ReferenceItem::Cache
  : std::vector<boost::variant<std::shared_ptr<smtk::resource::PersistentObject>,
      std::weak_ptr<smtk::resource::PersistentObject> > >
{
};

namespace
{
class access_reference
  : public boost::static_visitor<std::shared_ptr<smtk::resource::PersistentObject> >
{
public:
  const std::shared_ptr<smtk::resource::PersistentObject>& operator()(
    const std::shared_ptr<smtk::resource::PersistentObject>& persistentObject) const
  {
    return persistentObject;
  }

  std::shared_ptr<smtk::resource::PersistentObject> operator()(
    const std::weak_ptr<smtk::resource::PersistentObject>& weakPersistentObject) const
  {
    return weakPersistentObject.lock();
  }
};
}

struct ReferenceItem::const_iterator::CacheIterator : ReferenceItem::Cache::const_iterator
{
  CacheIterator() = default;
  CacheIterator(const ReferenceItem::Cache::const_iterator& it)
    : ReferenceItem::Cache::const_iterator(it)
  {
  }
  CacheIterator& operator=(ReferenceItem::Cache::const_iterator& it)
  {
    ReferenceItem::Cache::const_iterator::operator=(it);
    return *this;
  }
};

ReferenceItem::const_iterator::const_iterator()
  : m_cacheIterator(new ReferenceItem::const_iterator::CacheIterator)
{
}
ReferenceItem::const_iterator::const_iterator(const ReferenceItem::const_iterator& it)
  : m_cacheIterator(new ReferenceItem::const_iterator::CacheIterator(*(it.m_cacheIterator)))
{
}

ReferenceItem::const_iterator::~const_iterator() = default;

ReferenceItem::const_iterator& ReferenceItem::const_iterator::operator=(
  const ReferenceItem::const_iterator& it)
{
  m_cacheIterator.reset(new ReferenceItem::const_iterator::CacheIterator(*(it.m_cacheIterator)));
  return *this;
}

ReferenceItem::const_iterator& ReferenceItem::const_iterator::operator++()
{
  (*m_cacheIterator)++;
  return *this;
}
ReferenceItem::const_iterator& ReferenceItem::const_iterator::operator--()
{
  (*m_cacheIterator)--;
  return *this;
}

ReferenceItem::const_iterator ReferenceItem::const_iterator::operator++(int)
{
  operator++();
  return const_iterator(*this);
}
ReferenceItem::const_iterator ReferenceItem::const_iterator::operator--(int)
{
  operator--();
  return const_iterator(*this);
}

ReferenceItem::const_iterator ReferenceItem::const_iterator::operator+(
  const ReferenceItem::const_iterator::difference_type& d) const
{
  const_iterator returnValue(*this);
  *(returnValue.m_cacheIterator) += d;
  return returnValue;
}

ReferenceItem::const_iterator ReferenceItem::const_iterator::operator-(
  const ReferenceItem::const_iterator::difference_type& d) const
{
  const_iterator returnValue(*this);
  *(returnValue.m_cacheIterator) -= d;
  return returnValue;
}

ReferenceItem::const_iterator::reference ReferenceItem::const_iterator::operator*() const
{
  reference ref = boost::apply_visitor(access_reference(), *(*m_cacheIterator));
  if (ref == nullptr)
  {
    throw UnsetValueError();
  }
  return ref;
}
ReferenceItem::const_iterator::pointer ReferenceItem::const_iterator::operator->() const
{
  pointer ptr = boost::apply_visitor(access_reference(), *(*m_cacheIterator));
  if (ptr == nullptr)
  {
    throw UnsetValueError();
  }
  return ptr;
}
ReferenceItem::const_iterator::reference ReferenceItem::const_iterator::operator[](
  const difference_type& /*d*/)
{
  reference ref = boost::apply_visitor(access_reference(), *(*m_cacheIterator));
  if (ref == nullptr)
  {
    throw UnsetValueError();
  }
  return ref;
}

bool ReferenceItem::const_iterator::isSet() const
{
  reference ref = boost::apply_visitor(access_reference(), *(*m_cacheIterator));
  return (ref != nullptr);
}

ReferenceItem::const_iterator::difference_type operator-(
  const ReferenceItem::const_iterator& a, const ReferenceItem::const_iterator& b)
{
  return static_cast<ReferenceItem::const_iterator::difference_type>(
    *(a.m_cacheIterator) - *(b.m_cacheIterator));
}

bool operator<(const ReferenceItem::const_iterator& it1, const ReferenceItem::const_iterator& it2)
{
  return (*(it1.m_cacheIterator) < *(it2.m_cacheIterator));
}

bool operator>(const ReferenceItem::const_iterator& it1, const ReferenceItem::const_iterator& it2)
{
  return (*(it1.m_cacheIterator) > *(it2.m_cacheIterator));
}

bool operator<=(const ReferenceItem::const_iterator& it1, const ReferenceItem::const_iterator& it2)
{
  return !(it1 > it2);
}

bool operator>=(const ReferenceItem::const_iterator& it1, const ReferenceItem::const_iterator& it2)
{
  return !(it1 < it2);
}

bool operator==(const ReferenceItem::const_iterator& it1, const ReferenceItem::const_iterator& it2)
{
  return (*(it1.m_cacheIterator) == *(it2.m_cacheIterator));
}

bool operator!=(const ReferenceItem::const_iterator& it1, const ReferenceItem::const_iterator& it2)
{
  return !(it1 == it2);
}

ReferenceItem::ReferenceItem(Attribute* owningAttribute, int itemPosition)
  : Item(owningAttribute, itemPosition)
  , m_referencedAttribute(owningAttribute->shared_from_this())
  , m_cache(new Cache())
{
}

ReferenceItem::ReferenceItem(Item* inOwningItem, int itemPosition, int mySubGroupPosition)
  : Item(inOwningItem, itemPosition, mySubGroupPosition)
  , m_referencedAttribute(inOwningItem->attribute())
  , m_cache(new Cache())
{
}

ReferenceItem::ReferenceItem(const ReferenceItem& referenceItem)
  : Item(referenceItem)
  , m_referencedAttribute(referenceItem.m_referencedAttribute)
  , m_cache(new Cache(*referenceItem.m_cache))
{
}

ReferenceItem& ReferenceItem::operator=(const ReferenceItem& referenceItem)
{
  Item::operator=(referenceItem);
  m_referencedAttribute = referenceItem.m_referencedAttribute;
  m_cache.reset(new ReferenceItem::Cache(*(referenceItem.m_cache)));
  return *this;
}

ReferenceItem::~ReferenceItem()
{
  // Lets make sure the resource's links are updated
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt != nullptr)
  {
    std::size_t i, n = this->numberOfValues();
    for (i = 0; i < n; i++)
    {
      if (this->isSet(i))
      {
        this->unset(i);
      }
    }
  }
}

bool ReferenceItem::isValidInternal(
  bool useCategories, const std::set<std::string>& categories) const
{
  // If we have been given categories we need to see if the item passes its
  // category checks - if it doesn't it means its not be taken into account
  // for validity checking so just return true

  if (useCategories && !this->categories().passes(categories))
  {
    return true;
  }

  if (!this->isEnabled())
  {
    return true;
  }
  // Do we have at least the number of required values present?
  if (this->numberOfValues() < this->numberOfRequiredValues())
  {
    return false;
  }

  // Do all objects resolve to a valid reference?
  if (!this->resolve())
  {
    return false;
  }

  return true;
}

std::size_t ReferenceItem::numberOfValues() const
{
  return m_cache->size();
}

bool ReferenceItem::setNumberOfValues(std::size_t newSize)
{
  // If the current size is the same just return
  if (this->numberOfValues() == newSize)
  {
    return true;
  }

  // Next - are we allowed to change the number of values?
  auto def = static_cast<const ReferenceItemDefinition*>(m_definition.get());
  if (!def->isExtensible())
    return false; // You may not resize.

  // Next - are we within the prescribed limits?
  std::size_t n = def->numberOfRequiredValues();
  if (newSize < n)
    return false; // The number of values requested is too small.

  n = def->maxNumberOfValues();
  if (n > 0 && newSize > n)
    return false; // The number of values requested is too large.

  m_keys.resize(newSize);
  m_cache->resize(newSize);
  return true;
}

const std::multimap<std::string, std::string>& ReferenceItem::acceptableEntries() const
{
  auto def = this->definitionAs<ReferenceItemDefinition>();
  return def->acceptableEntries();
}

std::shared_ptr<const ReferenceItemDefinition> ReferenceItem::definition() const
{
  auto ptr =
    smtk::dynamic_pointer_cast<const smtk::attribute::ReferenceItemDefinition>(m_definition);
  return ptr;
}

std::size_t ReferenceItem::numberOfRequiredValues() const
{
  auto def = static_cast<const ReferenceItemDefinition*>(m_definition.get());
  if (!def)
  {
    return 0;
  }
  return def->numberOfRequiredValues();
}

std::size_t ReferenceItem::maxNumberOfValues() const
{
  auto def = static_cast<const ReferenceItemDefinition*>(m_definition.get());
  if (!def)
  {
    return 0;
  }
  return def->maxNumberOfValues();
}

bool ReferenceItem::contains(const smtk::common::UUID& entity) const
{
  return this->find(entity) >= 0;
}

bool ReferenceItem::contains(const PersistentObjectPtr& entity) const
{
  return this->find(entity) >= 0;
}

void ReferenceItem::visit(std::function<bool(const PersistentObjectPtr&)> visitor) const
{
  for (auto it = this->begin(); it != this->end(); ++it)
  {
    if (!it.isSet())
    {
      continue;
    }

    if (!visitor(*it))
    {
      break;
    }
  }
}

smtk::attribute::ReferenceItem::Key ReferenceItem::objectKey(std::size_t i) const
{
  if (i >= static_cast<std::size_t>(m_cache->size()))
    return Key();
  return m_keys[i];
}

bool ReferenceItem::setObjectKey(std::size_t i, const smtk::attribute::ReferenceItem::Key& key)
{
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if ((myAtt != nullptr) && (i < m_cache->size()))
  {
    myAtt->guardedLinks()->removeLink(m_keys[i]);
    m_keys[i] = key;
    return true;
  }
  return false;
}

smtk::resource::PersistentObjectPtr ReferenceItem::objectValue(std::size_t i) const
{
  return this->value(i);
}

smtk::resource::PersistentObjectPtr ReferenceItem::value(std::size_t i) const
{
  if (i >= static_cast<std::size_t>(m_cache->size()))
    return nullptr;

  auto result = boost::apply_visitor(access_reference(), (*m_cache)[i]);
  if (result == nullptr)
  {
    result = this->value(m_keys[i]);
    assignToCache(i, result);
  }
  return result;
}

bool ReferenceItem::setValue(const PersistentObjectPtr& val)
{
  return this->setValue(0, val);
}

ReferenceItem::Key ReferenceItem::linkTo(const PersistentObjectPtr& val)
{
  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());
  AttributePtr myAtt = this->m_referencedAttribute.lock();

  if (myAtt == nullptr)
  {
    return ReferenceItem::Key();
  }

  // If the object is a component...
  if (auto component = std::dynamic_pointer_cast<smtk::resource::Component>(val))
  {
    return myAtt->guardedLinks()->addLinkTo(component, def->role());
  }
  // If the object is a resource...
  else if (auto resource = std::dynamic_pointer_cast<smtk::resource::Resource>(val))
  {
    return myAtt->guardedLinks()->addLinkTo(resource, def->role());
  }

  // If the object cannot be cast to a resource or component, there's not much
  // we can do.
  return ReferenceItem::Key();
}

bool ReferenceItem::isValueValid(std::size_t i, const PersistentObjectPtr& val) const
{
  // is the size valid
  if (i >= this->numberOfValues())
  {
    return false;
  }
  if (val == nullptr)
  {
    // This value is always valid
    return true;
  }
  // Lets test its definition
  auto def = this->definitionAs<ReferenceItemDefinition>();
  if (!def->isValueValid(val))
  {
    return false;
  }

  // Is this the current value
  if (this->isSet(i) && (this->value(i) == val))
  {
    return true;
  }

  auto comp = std::dynamic_pointer_cast<smtk::resource::Component>(val);
  if (comp == nullptr)
  {
    return true;
  }

  // Else we need to check to see if its role is unique - this is currently supported
  // for resource component
  auto att = m_referencedAttribute.lock();
  if (att == nullptr)
  {
    return false;
  }

  auto attRes = att->attributeResource();
  if (attRes == nullptr)
  {
    return false;
  }

  if (!attRes->isRoleUnique(def->role()))
  {
    return true;
  }
  return (attRes->findAttribute(comp, def->role()) == nullptr);
}

bool ReferenceItem::setValue(std::size_t i, const PersistentObjectPtr& val)
{
  if (!this->isValueValid(i, val))
  {
    return false;
  }
  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());
  if ((i >= m_cache->size()) || (val != nullptr && !def->isValueValid(val)))
  {
    return false;
  }

  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt != nullptr)
  {
    myAtt->guardedLinks()->removeLink(m_keys[i]);
    m_keys[i] = this->linkTo(val);
  }
  else
  {
    m_keys[i] = Key();
  }

  assignToCache(i, val);
  return true;
}

bool ReferenceItem::appendValue(const PersistentObjectPtr& val)
{
  // First - is this value valid?
  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());
  if (!def->isValueValid(val))
  {
    return false;
  }

  // Second - is the value already in the item?
  std::size_t emptyIndex, n = this->numberOfValues();
  bool foundEmpty = false;
  for (std::size_t i = 0; i < n; ++i)
  {
    if (this->isSet(i) && (this->value(i) == val))
    {
      return true;
    }
    if (!this->isSet(i))
    {
      foundEmpty = true;
      emptyIndex = i;
    }
  }
  // If not, was there a space available?
  if (foundEmpty)
  {
    return this->setValue(emptyIndex, val);
  }
  // Finally - are we allowed to change the number of values?
  if ((def->isExtensible() && def->maxNumberOfValues() &&
        m_cache->size() >= def->maxNumberOfValues()) ||
    (!def->isExtensible() && m_cache->size() >= def->numberOfRequiredValues()))
  {
    // The number of values is fixed or we reached the max number of items
    return false;
  }

  m_keys.push_back(this->linkTo(val));
  appendToCache(val);
  return true;
}

bool ReferenceItem::removeValue(std::size_t i)
{
  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt == nullptr)
  {
    return false;
  }
  // If i < the required number of values this is the same as unset - else if
  // its extensible remove it completely
  if (i < def->numberOfRequiredValues())
  {
    this->unset(i);
    return true;
  }
  if (i >= this->numberOfValues())
  {
    return false; // i can't be greater than the number of values
  }
  myAtt->guardedLinks()->removeLink(m_keys[i]);
  m_keys.erase(m_keys.begin() + i);
  (*m_cache).erase((*m_cache).begin() + i);
  return true;
}

void ReferenceItem::detachOwningResource()
{
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt != nullptr)
  {
    // Populate the cache
    this->resolve();

    // Remove links to referenced items
    for (auto& key : m_keys)
    {
      myAtt->guardedLinks()->removeLink(key);
    }
  }

  // Flush keys
  m_keys.clear();
  m_keys.resize((*m_cache).size());

  // Let the base class detach from the resource
  Item::detachOwningResource();
}

void ReferenceItem::reset()
{
  // First remove all of the links being used
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt != nullptr)
  {
    // Remove links to referenced items
    for (auto& key : m_keys)
    {
      myAtt->guardedLinks()->removeLink(key);
    }
  }

  // Flush keys
  m_keys.clear();

  (*m_cache).clear();
  if (this->numberOfRequiredValues() > 0)
  {
    m_keys.resize(this->numberOfRequiredValues());
    (*m_cache).resize(this->numberOfRequiredValues());
  }
}

std::string ReferenceItem::valueAsString() const
{
  return this->valueAsString(0);
}

std::string ReferenceItem::valueAsString(std::size_t i) const
{
  std::ostringstream result;
  auto entry = this->value(i);
  smtk::resource::ResourcePtr rsrc;
  smtk::resource::ComponentPtr comp;
  if ((rsrc = std::dynamic_pointer_cast<smtk::resource::Resource>(entry)))
  {
    result << "resource(" << rsrc->id().toString() << ")";
  }
  else if ((comp = std::dynamic_pointer_cast<smtk::resource::Component>(entry)))
  {
    rsrc = comp->resource();
    smtk::common::UUID rid;
    if (rsrc)
    {
      rid = rsrc->id();
    }
    result << "component(" << rid << "," << comp->id() << ")";
  }
  return result.str();
}

bool ReferenceItem::isSet(std::size_t i) const
{
  return i < m_keys.size() ? !m_keys[i].first.isNull() : false;
}

void ReferenceItem::unset(std::size_t i)
{
  this->setValue(i, PersistentObjectPtr());
}

bool ReferenceItem::assign(ConstItemPtr& sourceItem, unsigned int options)
{
  // Cast input pointer to ReferenceItem
  auto sourceReferenceItem = smtk::dynamic_pointer_cast<const ReferenceItem>(sourceItem);
  if (!sourceReferenceItem)
  {
    return false; // Source is not a model entity item
  }
  // Are we supposed to assign the model enity values?
  if (options & Item::IGNORE_RESOURCE_COMPONENTS)
  {
    return Item::assign(sourceItem, options);
  }

  // Update values
  // Only set values if both att resources are using the same model
  this->setNumberOfValues(sourceReferenceItem->numberOfValues());
  for (std::size_t i = 0; i < sourceReferenceItem->numberOfValues(); ++i)
  {
    if (sourceReferenceItem->isSet(i))
    {
      auto val = sourceReferenceItem->value(i);
      this->setValue(i, val);
    }
    else
    {
      this->unset(i);
    }
  }
  return Item::assign(sourceItem, options);
}

bool ReferenceItem::isExtensible() const
{
  auto def = smtk::dynamic_pointer_cast<const ReferenceItemDefinition>(this->definition());
  if (!def)
    return false;
  return def->isExtensible();
}

ReferenceItem::const_iterator ReferenceItem::begin() const
{
  this->resolve();
  const_iterator it = const_iterator();
  ReferenceItem::Cache::const_iterator begin = m_cache->begin();
  *(it.m_cacheIterator) = begin;
  return it;
}

ReferenceItem::const_iterator ReferenceItem::end() const
{
  const_iterator it = const_iterator();
  ReferenceItem::Cache::const_iterator end = m_cache->end();
  *(it.m_cacheIterator) = end;
  return it;
}

std::ptrdiff_t ReferenceItem::find(const smtk::common::UUID& uid) const
{
  std::ptrdiff_t idx = 0;
  for (auto it = this->begin(); it != this->end(); ++it, ++idx)
  {
    if (!it.isSet())
    {
      continue;
    }

    if ((*it) && (*it)->id() == uid)
    {
      return idx;
    }
  }
  return -1;
}

std::ptrdiff_t ReferenceItem::find(const PersistentObjectPtr& comp) const
{
  std::ptrdiff_t idx = 0;
  for (auto it = this->begin(); it != this->end(); ++it, ++idx)
  {
    if (!it.isSet())
    {
      continue;
    }

    if ((*it) == comp)
    {
      return idx;
    }
    if ((*it) && (*it)->id() == comp->id())
    {
      std::cerr << "PROBLEM!!!  Found comp: " << comp->name() << " by ID\n";
    }
  }
  return -1;
}

smtk::resource::LockType ReferenceItem::lockType() const
{
  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());
  if (!def)
  {
    return smtk::resource::LockType::DoNotLock;
  }
  return def->lockType();
}

bool ReferenceItem::setDefinition(smtk::attribute::ConstItemDefinitionPtr adef)
{
  // Note that we do a dynamic cast here since we don't
  // know if the proper definition is being passed
  auto def = dynamic_cast<const ReferenceItemDefinition*>(adef.get());

  // Call the parent's set definition - similar to constructor calls
  // we call from base to derived
  if ((def == nullptr) || (!Superclass::setDefinition(adef)))
  {
    return false;
  }
  std::size_t n = def->numberOfRequiredValues();
  if (n != 0)
  {
    m_keys.resize(n);
    m_cache->resize(n);
  }
  return true;
}

smtk::resource::PersistentObjectPtr ReferenceItem::value(const ReferenceItem::Key& key) const
{
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt == nullptr)
  {
    return PersistentObjectPtr();
  }

  // We first try to resolve the item as a component.
  auto linkedObject = myAtt->guardedLinks()->linkedObject(key);
  if (linkedObject != nullptr)
  {
    // We can resolve the linked object.
    return linkedObject;
  }
  // If we cannot resolve the linked object, let's check to see if the object
  // is held by the same resource as this ReferenceItem. There's no need for
  // resource management in this event.
  else if (!key.first.isNull() &&
    myAtt->attributeResource()->guardedLinks()->resolve(myAtt->resource()))
  {
    return myAtt->guardedLinks()->linkedObject(key);
  }
  return PersistentObjectPtr();
}

bool ReferenceItem::resolve() const
{
  // This static value refers to an unset key. It is declared statically once
  // here rather than repeatedly created when querying whether the key is set.
  static const Key nullKey = Key();

  bool allResolved = true;
  AttributePtr myAtt = this->m_referencedAttribute.lock();
  if (myAtt == nullptr)
  {
    return false;
  }
  // We treat keys and values as vectors in lockstep with each other. If they
  // are not, then something unexpected has occured.
  assert(m_keys.size() == m_cache->size());

  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());

  // Iterate over the objects' keys and values.
  auto key = m_keys.begin();
  auto value = m_cache->begin();
  for (; value != m_cache->end(); ++value, ++key)
  {
    // If a value is not currently resolved...
    auto reference = boost::apply_visitor(access_reference(), *value);
    // TODO: There is a problem with resources being freed once they are no
    //       longer needed.  A side effect of this is that we cannot trust the
    //       weak pointer result since we are not sure if the returned object is
    //       not from an older version of the resource that was not properly
    //       deleted.  For the time being we will always look up the object if
    //       we are not explicitly holding the reference and the attribute
    //       resource itself is being managed.
    // if (reference == nullptr && (*key) != nullKey)
    if ((reference == nullptr) ||
      (!def->holdReference() && (*key) != nullKey && myAtt->resource()->manager() != nullptr))
    {
      // ...set it equal to the object pointer accessed using its key.
      auto newValue = this->value(*key);

      if (def->holdReference())
      {
        *value = std::shared_ptr<smtk::resource::PersistentObject>(newValue);
      }
      else
      {
        *value = std::weak_ptr<smtk::resource::PersistentObject>(newValue);
      }

      // If it's still not resolved...
      if (newValue == nullptr)
      {
        // ...there's not much we can do.
        allResolved = false;
      }
    }
  }

  return allResolved;
}

void ReferenceItem::assignToCache(std::size_t i, const PersistentObjectPtr& obj) const
{
  auto def = static_cast<const ReferenceItemDefinition*>(this->definition().get());

  if (def->holdReference())
  {
    (*m_cache)[i] = std::shared_ptr<smtk::resource::PersistentObject>(obj);
  }
  else
  {
    (*m_cache)[i] = std::weak_ptr<smtk::resource::PersistentObject>(obj);
  }
}

void ReferenceItem::appendToCache(const PersistentObjectPtr& obj) const
{
  std::size_t i = m_cache->size();
  m_cache->push_back(Cache::value_type());
  return assignToCache(i, obj);
}

template <>
bool ReferenceItem::iteratorIsSet<ReferenceItem::const_iterator>(
  const ReferenceItem::const_iterator& iterator) const
{
  return iterator.isSet();
}
}
}
