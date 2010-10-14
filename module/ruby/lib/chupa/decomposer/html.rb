require 'nokogiri'

class Chupa::HTML < Chupa::BaseDecomposer
  def decompose
    doc = Nokogiri::HTML.parse(read)
    self.metadata["title"] = (doc % "head/title").text
    self.metadata["charset"] = doc.encoding
    decomposed((doc % "body").text.gsub(/^\s+|\s+$/, ''))
  end
end
