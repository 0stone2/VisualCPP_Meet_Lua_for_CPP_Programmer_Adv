--[[
Lua (First PlugIn.lua)코드입니다.
--]]

--package.cpath = package.cpath .. ";"
package.path = package.path .. ";../?.lua"

local inspect = require ("inspect")

function GetInfo () 
	szName = "첫번째 플러그인"
	szDesc = "나의 첫번째 플러그인입니다"
	szVersion = "1.0.0"
	szDate = "2016.04.22"
	szAuthor = "박창진"
	szEmail = "crazevil@gmail.com"

	return szName, szDesc, szVersion, szDate, szAuthor, szEmail
end

function findLast(haystack, needle)
    local i=haystack:match(".*"..needle.."()")
    if i==nil then return nil else return i-1 end
end

function Rename (szSrcFile) 
	DbgString(szSrcFile)
	nIndex = findLast(szSrcFile, "\\");
	szFolder = string.sub(szSrcFile, 0, nIndex - 1)
	szFile = string.sub(szSrcFile, string.len(szFolder) + 2)

	DbgString("\n")
	DbgString("Folder:" .. szFolder .. "\n")
	DbgString("szFile:" .. szFile .. "\n")

	nIndex = findLast(szFile, "%.");
	szExt = string.sub(szFile, nIndex)
	szFile = string.sub(szFile, 0, string.len(szFile) - string.len(szExt))

	DbgString("\n")
	DbgString("Folder:" .. szFolder .. "\n")
	DbgString("szFile:" .. szFile .. "\n")
	DbgString("szExt:" .. szExt .. "\n")

	szNumber = string.match(szFile, "%d+")
	DbgString("szFile:" .. szNumber .. "\n")

	return szFolder .. "\\" .. szNumber .. szExt;
end

function RenameAll () 
	for nIndex = 1, #(leftPane.files), 1 do
		DbgString(leftPane.files[nIndex] .. "\n")
		file = Rename(leftPane.files[nIndex])

		DbgString("[before rightFile.addFile]>>" .. file .. "\n")
		rightPane:addFile(file, true)
		DbgString(">>" .. file .. "\n")
	end
end