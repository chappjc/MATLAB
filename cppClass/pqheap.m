classdef pqheap < cppclass
% Example class for how to derive from cppclass and interface with your C++ 
% class. Specifically, this demonstrates the use of a C++ priority queue.
% by Jonathan Chappelow (chappjc)
    methods
        
        % Use the name of your MEX file here
        function obj = pqheap(varargin)
            obj@cppclass('class_wrapper_template',varargin{:});
        end
        
        % new and delete are inherited, everything else calls cppmethod()
        
        % currentNumberOfElements = p.len()
        function varargout = len(obj)
            [varargout{1:nargout}] = obj.cppmethod('size');
        end
        
        % maxSize = p.capacity()
        function varargout = capacity(obj)
            [varargout{1:nargout}] = obj.cppmethod('capacity');
        end
        
        % position = p.insert(element)
        function varargout = insert(obj,element)
            if (obj.capacity()<1),
                error('pqheap:zeroCapacity','Zero capacity heap.'); end
            [varargout{1:nargout}] = obj.cppmethod('insert',element);
        end
        
        function fastInsert(obj,element)
            obj.cppmethod('insert',element);
        end
        
        % topElement = p.extract()
        function varargout = extract(obj)
            if (obj.len()<1),
                error('pqheap:heapEmpty','Heap is empty'); end
            [varargout{1:nargout}] = obj.cppmethod('extract');
        end
        
        % p.printHeap()
        function printHeap(obj)
            obj.cppmethod('print');
        end
        
        % init/reinit
        function varargout = init(obj,N)
            if (obj.len()>0),
                warning('pqheap:heapNotEmpty','Re-initializing non-empty heap'); end
            [varargout{1:nargout}] = obj.cppmethod('init',N);
        end
        
    end
        
end
