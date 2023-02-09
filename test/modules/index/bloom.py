from typing import Union, List, Tuple
# pip install bloom-filter
from bloom_filter import BloomFilter
import math
import re


def create_bloom_filter(len_data, precision=0.1):
    bloom_size = -((len_data * math.log(precision)) / (math.log(2)) ** 2)
    bloom = BloomFilter(max_elements=bloom_size, error_rate=precision)
    return bloom


def add_to_bloom_filter(bloom_filter, element):
    bloom_filter.add(element)
    return bloom_filter


def check_if_dictionary(element):
    return isinstance(element, dict)


def get_keys_of_element(element, attributes, parent_key):
    if check_if_dictionary(element):
        for elem_key in element.keys():
            attributes.add(parent_key + "/" + elem_key)
            attributes.union(get_keys_of_element(
                element[elem_key], attributes, parent_key + "/" + elem_key))

    return attributes


def bloom_from_state(state, predicate):
    all_attributes_are_present = True
    # extract the keys from the predicate
    text_between_quotations = re.findall("'(.*?)'", predicate)
    # check if the predicate is in the bloom filter
    for predicate_attribute in text_between_quotations:
        if predicate_attribute not in state["filter"]:
            all_attributes_are_present = False
            break
    if all_attributes_are_present:
        print("The predicate is in the bloom filter")
    else:
        print("The predicate is not in the bloom filter")
    return ([all_attributes_are_present] * state["num_docs"], not all_attributes_are_present)


def estimate_usage(predicate: str, state, doc_count: int) -> Union[None, int]:
    """
    Estimate the work remaining after the predicate has been evaluated with the index.
    None can be returned if the index is not applicable.
    """
    # if the predicate has OR then the index is not needed
    if "||" in predicate or "!" in predicate:
        return None

    return 0


def execute_state(predicate: str, state) -> Union[None, Tuple[List[bool], bool]]:
    """
    Execute the predicate using only the state.
    None can be returned if this is not possible.
    Else, a Tuple of (Document Index, Exact Match) is returned.
    If exact match is false, then the system will go over the result documents again and evaluate the predicate in a default manner
    """
    if "filter" in state:
        return bloom_from_state(state, predicate)
    return None


def execute_docs(predicate: str, state, docs) -> Tuple[List[bool], bool]:
    """
    Execute the predicate using the state and a list of all the documents.
    A Tuple of (Document Index, Exact Match) is returned.
    If exact match is false, then the system will go over the result documents again and evaluate the predicate in a default manner
    """
    if "filter" not in state:
        """
            create and fill the bloom filter with data
        """
        print("Creating bloomfilter")
        # store the documents size
        state["num_docs"] = len(docs)
        # create the bloom filter
        state["filter"] = create_bloom_filter(state["num_docs"], 0.1)
        # the list of all the keys from the documents
        all_keys = set()
        # fill the bloom filter
        for doc in docs:
            for doc_key in doc.keys():
                all_keys.add('/' + doc_key)
                # extract the keys from the document
                all_keys.union(get_keys_of_element(
                    doc[doc_key], all_keys, ('/' + doc_key)))

        # add the keys to the bloom filter
        for key in all_keys:
            state["filter"] = add_to_bloom_filter(state["filter"], key)
        print("Created bloomfilter")

    return bloom_from_state(state, predicate)


def improve_index(predicate: str, state, doc_index: List[bool]):
    """
    Improve the index using the final result of the query
    """
    return None


def init_index():
    """
    Returns the initial empty state of the index
    """

    return dict()
