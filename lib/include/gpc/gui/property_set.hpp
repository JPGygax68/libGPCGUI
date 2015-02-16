#pragma once

#include <map>

namespace gpc {

    namespace gui {

        template <typename Traits>
        struct PropertySet {

            typedef typename Traits::key_t key_t;
            typedef typename Traits::value_t value_t;

            static const key_t DEFAULT_KEY = Traits::DEFAULT_KEY;
            static const value_t DEFAULT_VALUE = Traits::DEFAULT_VALUE;

            struct Entry {
                bool defined;
                value_t value;
                Entry(const value_t value_): defined(true), value(value_) {}
                Entry(bool defined_ = false): defined(defined_) {}
            };

            PropertySet(const value_t def_val = DEFAULT_VALUE) {
                map[DEFAULT_KEY] = def_val;
            }

            void set(const key_t &key, const value_t &value) { map[key] = value; }

            void clear(const key_t &key) { map[key].defined = false; }

            value_t operator[] (const key_t &key) { 

                auto it = map.find(key);
                if (it != map.end() && it->second.defined) {
                    return it->second.value;
                }
                else {
                    return Traits::getDerivedValue(map[DEFAULT_KEY].value, key);
                }
            }

        private:
            std::map<key_t, Entry> map;
        };

    } // ns gui

} // ns gpc
