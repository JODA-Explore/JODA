from typing import Union, List, Tuple


def estimate_usage(predicate: str, state, doc_count: int) -> Union[None, int]:
    """
    Estimate the work remaining after the predicate has been evaluated with the index.
    None can be returned if the index is not applicable.
    """
    return 1 


def execute_state(predicate: str, state) -> Union[None, Tuple[List[bool],bool]]:
    """
    Execute the predicate using only the state.
    None can be returned if this is not possible.
    Else, a Tuple of (Document Index, Exact Match) is returned.
    If exact match is false, then the system will go over the result documents again and evaluate the predicate in a default manner
    """
    return None


def execute_docs(predicate: str, state, docs) -> Tuple[List[bool],bool]:
    """
    Execute the predicate using the state and a list of all the documents.
    A Tuple of (Document Index, Exact Match) is returned.
    If exact match is false, then the system will go over the result documents again and evaluate the predicate in a default manner
    """

    return None

def improve_index(predicate: str, state, doc_index: List[bool]):
    """
    Improve the index using the final result of the query
    """
    return

def init_index():
    """
    Returns the initial empty state of the index
    """

    return None