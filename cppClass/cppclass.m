classdef (Abstract = true) cppclass < handle
% cppclass: A base class for interfacing with a MEX file that wraps a C++ class
% by Jonathan Chappelow (chappjc)
% DO NOT EDIT. Derive from this class as demonstrated in:
%    https://github.com/chappjc/MATLAB/blob/master/cppClass/pqheap.m

    properties (GetAccess = private, SetAccess = immutable, Hidden = true, Transient = true)
        instanceHandle; % integer handle to a class instance in MEX function
    end
    
    properties (GetAccess = protected, SetAccess = immutable, Hidden = false)
        mexClassWrapperFnc; % the MEX function owning the class instances
    end
    
    methods
        
        function obj = cppclass(mexBackendFnc,varargin)
            
            if (nargin<1),
                error('cppclass:invalidConstruction',...
                    ['Must specify MEX-file backend (provides the actual class ' ...
                    'functionality by wrapping a C++ class)']);
            end
            
            obj.mexClassWrapperFnc = cppclass.checkMEXFnc(mexBackendFnc);
            
            % create a new C++ class instance for this MATLAB class instance
            obj.instanceHandle = obj.mexClassWrapperFnc('new',varargin{:});
            
        end

        function delete(obj)

            if ~isempty(obj.instanceHandle)
                obj.mexClassWrapperFnc('delete', obj.instanceHandle);
                obj.mexClassWrapperFnc = [];
                obj.instanceHandle = [];
            end
            
        end
        
    end
    
    methods (Sealed = true)
        
        function varargout = cppmethod(obj, methodName, varargin)
            if isempty(obj.instanceHandle),
                error('cppclass:invalidHandle','No class handle'); end
            [varargout{1:nargout}] = obj.mexClassWrapperFnc(methodName, obj.instanceHandle, varargin{:});
        end
        
    end
    
    methods (Static = true)
        
        function mexFnc = checkMEXFnc(mexFnc)
            % Input function_handle or name, return valid handle or error
            
            % accept string or function_handle
            if ischar(mexFnc),
                mexFnc = str2func(mexFnc);
            end
            
            % validate MEX-file function handle
            % http://stackoverflow.com/a/19307825/2778484
            if isa(mexFnc, 'function_handle'),
                funInfo = functions(mexFnc);
                if exist(funInfo.file,'file') ~= 3, % status 3 is MEX-file
                    error('cppclass:invalidMEXFunction','Invalid MEX file: "%s".',funInfo.file);
                end
            else
                error('cppclass:invalidFunctionHandle','Invalid function handle.');
            end
        end
        
    end
    
end
