require 'nokogiri'
class Chupa::HTML < Chupa
  def decompose
    doc = Nokogiri::HTML.parse(read)
    self.metadata["title"] = (doc % "head/title").text
    decomposed((doc % "body").text.strip)
  end
end
